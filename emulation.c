// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include <unicorn/unicorn.h>

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

#include "SDL.h"

#include "common.h"
#include "descriptor.h"
#include "emulation.h"
#include "exe.h"
#include "main.h"

FILE* handles[10000];
uint32_t handle_index = 1;
uint32_t hevent_index = 0;

//FIXME: These are hacks (register when mapping instead!)!
extern Exe* exe;
uint8_t* stack = NULL;
uint8_t* heap = NULL;

static uint32_t gdtAddress = 0xA0000000; //FIXME: Search somehow?!
static uint32_t gdtSize = 31 * sizeof(SegmentDescriptor); //FIXME: 31 came from the UC sample, why?!

static uint32_t tlsAddress = 0xB0000000; //FIXME: No idea where to put this yet
static uint32_t tlsSize = 0x1000;

static uint32_t stackAddress = 0xC0000000; // FIXME: Search free region instead..?
static uint32_t stackSize = 16 * 1024 * 1024; // 4 MiB stack should be PLENTY

#define HEAP_ADDRESS 0x0D000000
static uint32_t heapAddress = HEAP_ADDRESS;
static uint32_t heapSize = 1024 * 1024 * 1024; // 1024 MiB

static uc_engine *uc;
static uint32_t ucAlignment = 0x1000;


unsigned int currentThread = -1;
unsigned int threadCount = 0;

static ThreadContext* threads = NULL; //FIXME: Store pointers to threads instead? (Probably doesn't matter for re-volt)

static void TransferContext(ThreadContext* ctx, bool write) {
  enum uc_err(*transfer)(uc_engine*, uc_x86_reg, void*) = write ? uc_reg_write : uc_reg_read;
  transfer(uc, UC_X86_REG_EIP, &ctx->eip);
  transfer(uc, UC_X86_REG_ESP, &ctx->esp);
  transfer(uc, UC_X86_REG_EBP, &ctx->ebp);
  transfer(uc, UC_X86_REG_EAX, &ctx->eax);
  transfer(uc, UC_X86_REG_EBX, &ctx->ebx);
  transfer(uc, UC_X86_REG_ECX, &ctx->ecx);
  transfer(uc, UC_X86_REG_EDX, &ctx->edx);
  transfer(uc, UC_X86_REG_ESI, &ctx->esi);
  transfer(uc, UC_X86_REG_EDI, &ctx->edi);
  transfer(uc, UC_X86_REG_EFLAGS, &ctx->eflags);

  transfer(uc, UC_X86_REG_FPSW, &ctx->fpsw);
  transfer(uc, UC_X86_REG_FPCW, &ctx->fpcw);
  transfer(uc, UC_X86_REG_FPTAG, &ctx->fptw);
  //FIXME: Use REG_ST0 etc. here? They seem to contain garbage?!
  transfer(uc, UC_X86_REG_FP0, &ctx->fp[0]);
  transfer(uc, UC_X86_REG_FP1, &ctx->fp[1]);
  transfer(uc, UC_X86_REG_FP2, &ctx->fp[2]);
  transfer(uc, UC_X86_REG_FP3, &ctx->fp[3]);
  transfer(uc, UC_X86_REG_FP4, &ctx->fp[4]);
  transfer(uc, UC_X86_REG_FP5, &ctx->fp[5]);
  transfer(uc, UC_X86_REG_FP6, &ctx->fp[6]);
  transfer(uc, UC_X86_REG_FP7, &ctx->fp[7]);
}


static void PrintContext(ThreadContext* ctx) {
  info_printf("EIP: 0x%08" PRIX32 "\n", ctx->eip);
  info_printf("ESP: 0x%08" PRIX32 "\n", ctx->esp);
  info_printf("EBP: 0x%08" PRIX32 "\n", ctx->ebp);
  info_printf("EAX: 0x%08" PRIX32 "\n", ctx->eax);
  info_printf("EBX: 0x%08" PRIX32 "\n", ctx->ebx);
  info_printf("ECX: 0x%08" PRIX32 "\n", ctx->ecx);
  info_printf("EDX: 0x%08" PRIX32 "\n", ctx->edx);
  info_printf("ESI: 0x%08" PRIX32 "\n", ctx->esi);
  info_printf("EDI: 0x%08" PRIX32 "\n", ctx->edi);
  info_printf("EFLAGS: 0x%08" PRIX32 "\n", ctx->eflags);

  info_printf("FPSW: 0x%04" PRIX16 "\n", ctx->fpsw);
  info_printf("FPCW: 0x%04" PRIX16 "\n", ctx->fpcw);
  info_printf("FPTAG: 0x%04" PRIX16 "\n", ctx->fptw);

  for(unsigned int i = 0; i < 8; i++) {
    info_printf("FP%d: 0x", i);
    for(unsigned int j = 0; j < 10; j++) {
      info_printf("%02" PRIX8, ctx->fp[i].raw[9 - j]);
    }
    info_printf("\n");
  }
}



// Callback for tracing all kinds of memory errors
static void UcErrorHook(uc_engine* uc, uc_mem_type type, uint64_t address, int size, int64_t value, void* user_data) {
  char *type_string = "";
  switch (type) {
      case UC_MEM_READ: type_string = "Memory is read from"; break;
      case UC_MEM_WRITE: type_string = "Memory is written to"; break;
      case UC_MEM_FETCH: type_string = "Memory is fetched"; break;
      case UC_MEM_READ_UNMAPPED: type_string = "Unmapped memory is read from"; break;
      case UC_MEM_WRITE_UNMAPPED: type_string = "Unmapped memory is written to"; break;
      case UC_MEM_FETCH_UNMAPPED: type_string = "Unmapped memory is fetched"; break;
      case UC_MEM_WRITE_PROT: type_string = "Write to write protected, but mapped, memory"; break;
      case UC_MEM_READ_PROT: type_string = "Read from read protected, but mapped, memory"; break;
      case UC_MEM_FETCH_PROT: type_string = "Fetch from non-executable, but mapped, memory"; break;
      case UC_MEM_READ_AFTER: type_string = "Memory is read from (successful access)"; break;
  };
  sys_printf("Unicorn-Engine error of type %d %s at 0x%" PRIx64 ", size = 0x%" PRIX32 "\n", type, type_string, address, size);
  uc_emu_stop(uc);

  ThreadContext ctx;
  TransferContext(&ctx, false);
  PrintContext(&ctx);

  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, &eip);
  sys_printf("Emulation returned %X\n", eip);

  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, &esp);
  for(int i = 0; i < 100; i++) {
    sys_printf("Stack [%d] = %X\n", i, *(uint32_t*)Memory(esp + i * 4));
  }

  assert(false);
}

// Callback for tracing instructions
static void UcTraceHook(void* uc, uint64_t address, uint32_t size, void* user_data) {
  int eip, esp, eax, esi;
  uc_reg_read(uc, UC_X86_REG_EIP, &eip);
  uc_reg_read(uc, UC_X86_REG_ESP, &esp);
  uc_reg_read(uc, UC_X86_REG_EAX, &eax);
  uc_reg_read(uc, UC_X86_REG_ESI, &esi);
  static uint32_t id = 0;
  info_printf("%7" PRIu32 " TRACE Emulation at 0x%X (ESP: 0x%X); eax = 0x%08" PRIX32 " esi = 0x%08" PRIX32 " (TS: %" PRIu64 ")\n", id++, eip, esp, eax, esi, SDL_GetTicks());
}

typedef struct {
  bool is_called;
  bool is_block_enter;
  bool is_block_exit;
  uint64_t count;
  uint64_t duration;
} Heat;

// Contains 0x10000 pointers to heat pages (each 0x10000 elements)
static Heat** heat = NULL;
static bool heat_is_block_enter_next = false;
static bool heat_is_called_next = false;
static uint32_t heat_address = 0;

// Callback for profiling instructions
static void UcProfilingBlockHook(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
  heat_is_block_enter_next = true;
  // Check where we originated and what kind of instruction it is
  if (heat_address != 0) {
    uint8_t* opcode = (uint8_t*)Memory(heat_address);
    if (opcode[0] == 0x9A) {
      heat_is_called_next = true;
    } else if (opcode[0] == 0xE8) {
      heat_is_called_next = true;
    } else if (opcode[0] == 0xFF) {
      uint8_t mod = opcode[1] >> 3;
      heat_is_called_next = ((mod == 2) || (mod == 3));
    }
  }
}

static void UcProfilingHook(void* uc, uint64_t address, uint32_t size, void* user_data) {
  static Uint64 instruction_started = 0;
  static bool is_called = false;
  static bool is_block_enter = false;
  if (heat_address != 0) {
    Uint64 instruction_finished = SDL_GetPerformanceCounter();

    uint64_t duration = instruction_finished - instruction_started;
    duration *= 1000000000ULL;
    duration /= SDL_GetPerformanceFrequency();

    if (heat == NULL) {
      heat = malloc(0x10000 * sizeof(Heat*));
      memset(heat, 0x00, 0x10000 * sizeof(Heat*));
    }

    uint32_t page = heat_address >> 16;
    uint32_t index = heat_address & 0xFFFF;
    if (heat[page] == NULL) {
      heat[page] = malloc(0x10000 * sizeof(Heat));
      memset(heat[page], 0x00, 0x10000 * sizeof(Heat));
    }
    Heat* h = &heat[page][index];
    h->count += 1;
    h->duration += duration;
    h->is_called |= is_called;
    h->is_block_enter |= is_block_enter;
    h->is_block_exit |= heat_is_block_enter_next;
  }
  instruction_started = SDL_GetPerformanceCounter();
  heat_address = address;
  is_called = heat_is_called_next;
  is_block_enter = heat_is_block_enter_next;
  
  // Assume that the next instruction won't be a block / called again.
  // The block handler will signal it again if that's the case.
  heat_is_called_next = false;
  heat_is_block_enter_next = false;
}

void DumpProfilingHeat(const char* path) {
  FILE* f;
  if (path == NULL) {
    f = stdout;
  } else {
    f = fopen(path, "w");
  }

  for(uint32_t page = 0; page < 0x10000; page++) {
    if (heat[page] == NULL) {
      continue;
    }
    for(uint32_t index = 0; index < 0x10000; index++) {
      Heat* h = &heat[page][index];
      if (h->count == 0) {
        continue;
      }
      fprintf(f, "PROF 0x%08" PRIX32 " %14" PRIu64 " %14" PRIu64 "%s%s%s\n",
              (page << 16) | index,
              h->count,
              h->duration,
              h->is_called ? " CALLED" : "",
              h->is_block_enter ? " BLOCK_ENTER" : "",
              h->is_block_exit ? " BLOCK_EXIT" : "");
    }
  }

  if (f != stdout) {
    fclose(f);
  }
}

void MapMemory(void* memory, uint32_t address, uint32_t size, bool read, bool write, bool execute) {
  //FIXME: Permissions!
  uc_err err;
  assert(size % ucAlignment == 0);
  err = uc_mem_map_ptr(uc, address, size, UC_PROT_ALL, memory);
  if (err) {
    info_printf("Failed on uc_mem_map_ptr() with error returned %u: %s\n", err, uc_strerror(err));
  }
  //FIXME: Add to mapped memory list
}

Address Allocate(Size size) {
  static uint32_t address = HEAP_ADDRESS;
  uint32_t ret = address;
  address += size;
#if 1
  // Debug memset to detect memory errors
  memset(Memory(ret), 0xDD, size);
#endif
  //FIXME: Proper allocator

#if 1
//FIXME: This is a hack to fix alignment + to avoid too small allocations
address += 0x1000;
address &= 0xFFFFF000;
#endif

  return ret;
}

void Free(Address address) {
  //FIXME!
}

void* Memory(uint32_t address) {

  if (address >= heapAddress && address < (heapAddress + heapSize)) {
    return &heap[address - heapAddress];
  }

  if (address >= stackAddress && address < (stackAddress + stackSize)) {
    return &stack[address - stackAddress];
  }

  if (address >= exe->peHeader.imageBase) {
    address -= exe->peHeader.imageBase;
    for(unsigned int sectionIndex = 0; sectionIndex < exe->coffHeader.numberOfSections; sectionIndex++) {
      PeSection* section = &exe->sections[sectionIndex];
      if ((address >= section->virtualAddress) && (address < (section->virtualAddress + section->virtualSize))) {
        assert(exe->mappedSections[sectionIndex] != NULL);
        uint32_t offset = address - section->virtualAddress;
        return &exe->mappedSections[sectionIndex][offset];
      }
    }
  }

  return NULL;
}

Address CreateHlt() {
  Address code_address = Allocate(2);
  uint8_t* code = Memory(code_address);
  *code++ = 0xF4; // HLT
  //FIXME: Are changes to regs even registered here?!
  *code++ = 0xC3; // End block with RET
  return code_address;
}

Address CreateInt21() {
  Address code_address = Allocate(3);
  sys_printf("Interrupt handler 21h at 0x%08x\n", code_address);
  uint8_t* code = Memory(code_address);
  *code++ = 0xCD; // INT
  *code++ = 0x21; // 21h;
  *code++ = 0xC3; // End block with RET
  return code_address;
}

Address CreateInt(uint32_t intno, uint32_t eax) {
  Address code_address = Allocate(10);
  sys_printf("Interrupt handler %xh:%x at 0x%08x\n", intno, eax, code_address);
  uint8_t* code = Memory(code_address);
  *code++ = 0x50; // PUSH EAX;
  if (eax) {
    *code++ = 0xB8; // MOV EAX,
    *(uint32_t*)code = eax; // eax; (TODO make sure: little endian)
    code += sizeof(uint32_t);
  } else {
    *code++ = 0x31; // XOR
    *code++ = 0xC0; // EAX, EAX;
  }
  if (intno == 3) {
    *code++ = 0xCC; // INT 3
  } else {
    *code++ = 0xCD; // INT
    *code++ = intno; // intno;
  }
  *code++ = 0x58; // POP EAX;
  *code++ = 0xC3; // End block with RET
  return code_address;
}

typedef struct {
  Address address;
  void(*callback)(void* uc, Address address, void* user_data);
  void* user_data;
} HltHandler;
HltHandler* hltHandlers = NULL;
unsigned int hltHandlerCount = 0;

int compareHltHandlers(const void * a, const void * b) {
  return ((HltHandler*)a)->address - ((HltHandler*)b)->address;
}

HltHandler* findHltHandler(Address address) {
  return bsearch(&address, hltHandlers, hltHandlerCount, sizeof(HltHandler), compareHltHandlers);
}

void AddHltHandler(Address address, void(*callback)(void* uc, Address address, void* user_data), void* user_data) {
  HltHandler* handler = findHltHandler(address);
  assert(handler == NULL); // Currently only supporting one handler

  hltHandlers = realloc(hltHandlers, ++hltHandlerCount * sizeof(HltHandler));
  handler = &hltHandlers[hltHandlerCount - 1];
  handler->address = address;
  handler->callback = callback;
  handler->user_data = user_data;

  // Resort the array, it will be binary-searched later
  qsort(hltHandlers, hltHandlerCount, sizeof(HltHandler), compareHltHandlers);
}

#if 0

//FIXME: Bad to use allocate in this file..?
Address CreateCallback(void* callback, void* user) {
  //FIXME: This might be faster, but needs some more work..
  Address address = Allocate(1 + 1 + strlen(user) + 1);
  static bool hooked = false;
  if (hooked == false) {
    uc_hook interruptHook;
    uc_hook_add(uc, &interruptHook, UC_HOOK_INTR, callback, NULL, 1, 0);
    hooked = true;
  }
  uint8_t* code = Memory(address);
  code[0] = 0xCC;
  code[1] = 0xC3;
  strcpy(&code[2], user);
  
  return address;
}

#endif

void UcInterruptHook(uc_engine *uc, uint32_t intno, void *user_data)
{
    bool silent = false;
    
    int32_t eip;
    uc_reg_read(uc, UC_X86_REG_EIP, &eip);
    int32_t esp;
    uc_reg_read(uc, UC_X86_REG_ESP, &esp);
    int32_t eax;
    uc_reg_read(uc, UC_X86_REG_EAX, &eax);
    
    // Address stackAddress = esp;
    uint32_t* stack = (uint32_t*)Memory(esp);
    Address returnAddress = stack[0];
    
    /* This lists a stack trace. */
    /* It's a failed attempt because most functions omit the frame pointer */
    /*int ebp;
     uc_reg_read(uc, UC_X86_REG_EBP, &ebp);
     StackTrace(ebp, 10, 4); */
    
    //sys_printf(">>> 0x%x: interrupt 0x%x, EAX = 0x%x, ESP = 0x%x\n", eip, intno, eax, esp);

    if (intno == 0x21) {
      Address hltAddress = eip;
      HltHandler* hltHandler = findHltHandler(hltAddress);
      if(hltHandler != NULL) {
        hltHandler->callback(uc, hltHandler->address, hltHandler->user_data);
      }
    }
}

void InitializeEmulation() {

  uc_err err;

  err = uc_open(UC_ARCH_X86, UC_MODE_32, &uc);
  if (err) {
    info_printf("Failed on uc_open() with error returned %u: %s\n", err, uc_strerror(err));
  }

#ifndef UC_KVM
  // Add hooks to catch errors
  uc_hook errorHooks[6];
  {
    // Hook for memory read on unmapped memory
    uc_hook_add(uc, &errorHooks[0], UC_HOOK_MEM_READ_UNMAPPED, UcErrorHook, NULL, 1, 0);

    // Hook for invalid memory write events
    uc_hook_add(uc, &errorHooks[1], UC_HOOK_MEM_WRITE_UNMAPPED, UcErrorHook, NULL, 1, 0);

    // Hook for invalid memory fetch for execution events
    uc_hook_add(uc, &errorHooks[2], UC_HOOK_MEM_FETCH_UNMAPPED, UcErrorHook, NULL, 1, 0);

    // Hook for memory read on read-protected memory
    uc_hook_add(uc, &errorHooks[3], UC_HOOK_MEM_READ_PROT, UcErrorHook, NULL, 1, 0);

    // Hook for memory write on write-protected memory
    uc_hook_add(uc, &errorHooks[4], UC_HOOK_MEM_WRITE_PROT, UcErrorHook, NULL, 1, 0);

    // Hook for memory fetch on non-executable memory
    uc_hook_add(uc, &errorHooks[5], UC_HOOK_MEM_FETCH_PROT, UcErrorHook, NULL, 1, 0);
  }
    
  uc_hook interruptHook;
  uc_hook_add(uc, &interruptHook, UC_HOOK_INTR, UcInterruptHook, NULL, 1, 0);
#endif

#ifndef UC_KVM
  // Setup segments
  SegmentDescriptor* gdtEntries = (SegmentDescriptor*)aligned_malloc(ucAlignment, AlignUp(gdtSize, ucAlignment));
  memset(gdtEntries, 0x00, gdtSize);

  gdtEntries[14] = CreateDescriptor(0x00000000, 0xFFFFF000, true);  // CS
  gdtEntries[15] = CreateDescriptor(0x00000000, 0xFFFFF000, false);  // DS
  gdtEntries[16] = CreateDescriptor(tlsAddress, tlsSize - 1, false); // FS

  //FIXME: Remove? We never switch to ring 0 anyway (Came from UC sample code)
  gdtEntries[17] = CreateDescriptor(0x00000000, 0xFFFFF000, false);  // Ring 0
  gdtEntries[17].dpl = 0;  //set descriptor privilege level

  err = uc_mem_map_ptr(uc, gdtAddress, AlignUp(gdtSize, ucAlignment), UC_PROT_WRITE | UC_PROT_READ, gdtEntries);

  uc_x86_mmr gdtr;
  gdtr.base = gdtAddress;  
  gdtr.limit = gdtSize - 1;
  err = uc_reg_write(uc, UC_X86_REG_GDTR, &gdtr);

  int cs = 0x73;
  err = uc_reg_write(uc, UC_X86_REG_CS, &cs);

  int ds = 0x7B;
  err = uc_reg_write(uc, UC_X86_REG_DS, &ds);

  int es = 0x7B;
  err = uc_reg_write(uc, UC_X86_REG_ES, &es);

  int fs = 0x83;
  err = uc_reg_write(uc, UC_X86_REG_FS, &fs);

  //FIXME: Do we require GS?!

//  int ss = 0x7B;
  int ss = 0x88; // Ring 0 - Why?!
  err = uc_reg_write(uc, UC_X86_REG_SS, &ss);
#endif

#if 0
  //FIXME: Steal actual register values, consult Windows ABI
  //https://github.com/corkami/docs/blob/master/InitialValues.md
  int eax;
  int ebx;
  int ecx;
  int edx;
  uc_reg_write(uc, UC_X86_REG_EAX, &eax);
  uc_reg_write(uc, UC_X86_REG_EBX, &ebx);
  uc_reg_write(uc, UC_X86_REG_ECX, &ecx);
  uc_reg_write(uc, UC_X86_REG_EDX, &edx);
#endif

  // Map and set TLS (not exposed via flat memory)
  uint8_t* tls = aligned_malloc(ucAlignment, tlsSize);
  memset(tls, 0xBB, tlsSize);
  err = uc_mem_map_ptr(uc, tlsAddress, tlsSize, UC_PROT_WRITE | UC_PROT_READ, tls);

  // Allocate a heap
  heap = aligned_malloc(ucAlignment, heapSize);
  memset(heap, 0xAA, heapSize);
  MapMemory(heap, heapAddress, heapSize, true, true, true);
}

void SetTracing(bool enabled) {
  // Add a trace hook so we get proper EIP after running
  static uc_hook traceHook = -1;
  if (enabled) {
    if (traceHook == -1) {
      uc_hook_add(uc, &traceHook, UC_HOOK_CODE, UcTraceHook, NULL, 1, 0);
    }
  } else {
    if (traceHook != -1) {
      uc_hook_del(uc, traceHook);
      traceHook = -1;
    }
  }
}

void SetProfiling(bool enabled) {

  // First, clear the old heatmap if it exists
  if (heat != NULL) {
    for(uint32_t page = 0; page < 0x10000; page++) {
      if (heat[page] != NULL) {
        free(heat[page]);
      }
    }
    free(heat);
    heat = NULL;

    // Setting address to zero signals that no profiling sample has started
    heat_address = 0;

    info_printf("Profiling heat has been cleared\n");
  }

  static uc_hook profilingBlockHook = -1;
  static uc_hook profilingHook = -1;
  if (enabled) {
    if (profilingBlockHook == -1) {
      uc_hook_add(uc, &profilingBlockHook, UC_HOOK_BLOCK, UcProfilingBlockHook, NULL, 1, 0);
    }
    if (profilingHook == -1) {
      uc_hook_add(uc, &profilingHook, UC_HOOK_CODE, UcProfilingHook, NULL, 1, 0);
    }
  } else {
    if (profilingBlockHook != -1) {
      uc_hook_del(uc, profilingBlockHook);
      profilingBlockHook = -1;
    }
    if (profilingHook != -1) {
      uc_hook_del(uc, profilingHook);
      profilingHook = -1;
    }
  }
}

Address CreateThreadBegin()
{
    Address symAddress = 0;
    Export* export = LookupExportByName("ExitThread");
    if (export) {
        if (export->address == 0) {
            symAddress = CreateInt21();
            AddHltHandler(symAddress, export->callback, export->name);
            export->address = symAddress;
        } else {
            symAddress = export->address;
        }
    }
    
    Address thread_begin = Allocate(10);
    uint8_t* code = Memory(thread_begin);
    *code++ = 0xFF; // CALL
    *code++ = 0xD0; //   EAX;
    if (symAddress) {
        *code++ = 0xB8; // MOV EAX,
        *(uint32_t*)code = symAddress; code += sizeof(uint32_t);
        *code++ = 0xFF; // CALL
        *code++ = 0xD0; //   EAX;
    }
    *code++ = 0xF4; // HLT;
    
    return thread_begin;
}

unsigned int CreateEmulatedThread(uint32_t eip, bool suspended) {
  static Address thread_begin = 0;
  if (thread_begin == 0) thread_begin = CreateThreadBegin();
  static int threadId = 0;
    
  threadId++;
  threads = realloc(threads, ++threadCount * sizeof(ThreadContext));
  ThreadContext* ctx = &threads[threadCount - 1];
  ctx->id = threadId;
    
  //FIXME: Dirty hack!
  // Map and set stack
  //FIXME: Use requested size
  if (stack == NULL) {
    stack = aligned_malloc(ucAlignment, stackSize);
    MapMemory(stack, stackAddress, stackSize, true, true, false);
  }
    
  uint32_t esp = stackAddress + stackSize / 2 + 256 * 1024 * threadId; // 256 kiB per late thread
  assert(threadId < 4);
  
  TransferContext(ctx, false /* read */); //FIXME: Find safe defaults instead?!
  ctx->eip = thread_begin;
  ctx->eax = eip;
  ctx->esp = esp;
  ctx->ebp = 0;
  ctx->sleep = 0;
  ctx->active = true;
  ctx->running = !suspended;
    
  PrintContext(ctx);

  return ctx->id;
}

void SleepThread(uint64_t duration) {
  GetCurrentThreadContext()->sleep = duration;
  // FIXME: multi-thread uc?
  //uc_emu_stop(uc);
}

void DeleteEmulatedThread() {
  //FIXME: How to deal with deletion of the running thread?
  GetCurrentThreadContext()->active = false;
}

static unsigned int GetThreadCount() {
  //FIXME: Protect with mutex?!
  return threadCount;
}

ThreadContext *GetCurrentThreadContext() {
    return &threads[currentThread];
}

void RunEmulation() {
  uc_err err;

  //FIXME: plenty of options to optimize in single threaded mode.. (register readback not necessary etc.)
  int runningThreads = 0;
  while(GetThreadCount() > 0) {

    // Very simple round robin schedule.. Re-Volt only uses threads during load screens anyway..
    currentThread++;
    if (currentThread >= threadCount) {
      if (runningThreads == 0) break;
      currentThread = 0;
      runningThreads = 0;
    }

    // Get current thread
    ThreadContext* ctx = &threads[currentThread];
    
    if (!ctx->active) continue;
    if (!ctx->running) continue;
    runningThreads++;

    //FIXME: Decrement time by time slice instead..
    if (ctx->sleep > 0) {
      info_printf("\n\n\n\n\nNot waking thread %d from sleep yet\n\n\n\n\n\n", currentThread);
      ctx->sleep -= 1;
      continue;
    }

    TransferContext(ctx, true /* write */);

    uint32_t lastTime = SDL_GetTicks();
    while(ctx->active && ctx->running) {
      err = uc_emu_start(uc, ctx->eip, 0, 0, 3000000);

      // Finish profiling, if we have partial data
      if (heat_address != 0) {
        // Signal block exit by marking the next instruction as block entry
        heat_is_block_enter_next = true;

        // Save the last profiling sample
        UcProfilingHook(uc, 0, 0, NULL);

        // Setting address to zero signals that no profiling sample has started
        heat_address = 0;
      }

      // Check for errors
      if (err != 0) {
        break;
      }

      uint32_t currentTime = SDL_GetTicks();
    
      uc_reg_read(uc, UC_X86_REG_EIP, &ctx->eip);

      Address hltAddress = ctx->eip - 1;
    
      if (*(uint8_t*)Memory(hltAddress) == 0xF4) {
          HltHandler* hltHandler = findHltHandler(hltAddress);
          if(hltHandler != NULL) {
            hltHandler->callback(uc, hltHandler->address, hltHandler->user_data);
          }

          //Hack: Manually transfers EIP (might have been changed in callback)
          uc_reg_read(uc, UC_X86_REG_EIP, &ctx->eip);
      }
      
      if (currentTime > lastTime + 333) {
        sys_printf("<%d> Timeout EIP = 0x%x, ticks = %d\n", ctx->id, ctx->eip, currentTime - lastTime);
        break; // Context switch after several ticks (ms)
      }
    }

    // threads array might be relocated if a thread was modified in a callback; update ctx pointer
    ctx = &threads[currentThread];

    TransferContext(ctx, false /* read */);

    if (err != 0) {
      sys_printf("Failed on uc_emu_start() with error returned %u: %s\n", err, uc_strerror(err));
      PrintContext(ctx);
      assert(false);
    }

    //sys_printf("\n\n\Emulation slice completed for thread %d (Count: %d) with %d at 0x%X\n", currentThread, threadCount, err, ctx->eip);

    //PrintContext(ctx);
    //info_printf("\n\n\n\n\n");
  }
}

void CleanupEmulation(void) {
  uc_close(uc);
}
