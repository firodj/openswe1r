// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "main.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <ctype.h>
#include <thread>
#include <memory>

#include "common.h"
#include "descriptor.h"
#include "emulation.h"
#include "exe.h"

//FIXME: Alternative for non-posix OS!
#include <time.h>

#include "Application.hpp"
#include "StarWars.hpp"

#include "com/d3d.h"
#include "com/ddraw.h"
#include "com/dinput.h"


uint32_t callId = 0;

unsigned int exportCount = 0;
Export* exports = NULL;

void AddExport(const char* name, ExportCallback callback, Address address) {
  exports = (Export*)realloc(exports, (exportCount + 1) * sizeof(Export));
  Export* export_sym = &exports[exportCount];
  export_sym->name = (const char*)malloc(strlen(name) + 1);
  strcpy((char*)export_sym->name, name);
  export_sym->callback = callback;
  export_sym->address = address;
  export_sym->thunkAddress = 0;
  export_sym->hook = 1;
  exportCount++;
}

void AddExport2(const char* name, ExportCallback callback) {
  exports = (Export*)realloc(exports, (exportCount + 1) * sizeof(Export));
  Export* export_sym = &exports[exportCount];
  export_sym->name = (const char*)malloc(strlen(name) + 1);
  strcpy((char*)export_sym->name, name);
  export_sym->callback = callback;
  export_sym->address = 0;
  export_sym->thunkAddress = 0;
  export_sym->hook = 2;
  exportCount++;
}

Export* LookupExportByName(const char* name) {
  for(unsigned int i = 0; i < exportCount; i++) {
    Export* export_sym = &exports[i];
    if (!strcmp(export_sym->name, name)) {
      return export_sym;
    }
  }
  return NULL;
}


// HACK BLOCK!

const char** dirlisting = NULL;

uint32_t tls[1000] = {0};

//FIXME: To be moved elsewhere
#include "shaders.h"
#include "shader.h"


#include "windows.h" // Hack while exports are not ready
// HACK:
#include <unicorn/unicorn.h>

void UnknownImport(uc_engine* uc, Address address, void* user_data);

Address CreateInterface(const char* name, unsigned int slotCount, uint32_t objectSize) {
  //FIXME: Unsure about most terminology / inner workings here
  Address interfaceAddress = Allocate(objectSize);
  Address vtableAddress = Allocate(4 * slotCount);
  uint32_t* vtable = (uint32_t*)Memory(vtableAddress);
  for(unsigned int i = 0; i < slotCount; i++) {
    // Point addresses to themself
    char* slotName = (char*)malloc(128);
    sprintf(slotName, "%s__%d", name, i);
    Export* export_sym = LookupExportByName(slotName);

    Address hltAddress;
    if (export_sym != NULL) {
      if (export_sym->address == 0) {
        hltAddress = CreateHlt();
        AddHltHandler(hltAddress, export_sym->callback, slotName);
        export_sym->address = hltAddress;
      } else {
        hltAddress = export_sym->address;
      }
    } else {
      hltAddress = CreateHlt();
      AddHltHandler(hltAddress, UnknownImport, slotName);
      AddExport((const char*)slotName, UnknownImport, hltAddress);
    }
    vtable[i] = hltAddress;
  }
  // First element in object is pointer to vtable
  *(uint32_t*)Memory(interfaceAddress) = vtableAddress;

  return interfaceAddress;
}

char* TranslatePath(const char* path) {
  size_t length = strlen(path) + 1;
  char* newPath = (char*)malloc(length);
  char* cursor = strcpy(newPath, path);
  while(*cursor != '\0') {
    if (*cursor == '\\') {
      *cursor = '/';
    } else {
      *cursor = tolower(*cursor);
    }
    cursor++;
  }

  // This is a patch for the original CD release.
  // We simply map the "/gnome/data/" folder from the CD back to our current
  // folders "/data/" folder.
  // This allows easier installation (by simply copying all files from disc)
  if ((length >= 14) && !memcmp(newPath, "d:/gnome/data/", 14)) {
    memcpy(newPath, "./data/", 7);
    memmove(&newPath[7], &newPath[14], length - 14);
    return newPath;
  }

  // This is another CD patch for some patched games.
  // They search CD contents directly in the CDs "/data/" path (instead of
  // "/gnome/data/"). This allows them to just set the "CD Path" to the
  // installation folder of the game.
  // The most prominent version doing this is probably the GOG.com re-release
  // from 2018.
  if ((length >= 8) && !memcmp(newPath, "d:/data/", 8)) {
    memcpy(newPath, "./data/", 7);
    memmove(&newPath[7], &newPath[8], length - 8);
    return newPath;
  }

  if ((length >= 10) && !memcmp(newPath, "d:/./data/", 10)) {
    memcpy(newPath, "./data/", 7);
    memmove(&newPath[7], &newPath[10], length - 10);
    return newPath;
  }

  return newPath;
}

void StackTrace(uint32_t base, unsigned int frames, unsigned int arguments) {
  uint32_t stackAddress = base;
  for(unsigned int i = 0; i < frames; i++) {
    info_printf("Base: 0x%" PRIX32 "\n", stackAddress);
    if (stackAddress == 0) {
      // End of stack trace!
      return;
    }
    uint32_t* stack = (uint32_t*)Memory(stackAddress);
    // stack[0] = EBP of calling function
    // stack[1] = Return address
    // stack[2..] = Arguments
    if (stack == NULL) {
      info_printf("Corrupt base in trace!\n");
      return;
    }
    info_printf("#%2d Returns to 0x%" PRIX32 " (", i, stack[1]);
    for(unsigned int j = 0; j < arguments; j++) {
      info_printf("@%d=0x%08" PRIX32 ", ", j, stack[j + 2]);
    }
    info_printf("...)\n");
    // Get the previous ebp
    stackAddress = stack[0];
  }
}



static void UcTimerHook(void* uc, uint64_t address, uint32_t size, void* user_data) {
  info_printf("Time is %" PRIu64 "\n", GetTicks());
}

// This is strictly for debug purposes, it attempts to dump fscanf (internally used by sscanf too)
static void UcFscanfHook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
  info_printf("\nfscanf\n\n");

  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);


  // This is the FILE struct used by microsoft CRT
  typedef struct {
    Address _ptr;
    int32_t _cnt;
    Address _base;
    int32_t _flag;
    int32_t _file;
    int32_t _charbuf;
    int32_t _bufsiz;
    Address _tmpfname;
  } _iobuf;

  // Pop the return address
  Address returnAddress = stack[0];
  info_printf("Return at 0x%" PRIX32 "\n", returnAddress);
  _iobuf* iob = (_iobuf*)Memory(stack[1]); // Get FILE object
  char* buf = (char*)Memory(iob->_ptr);
  info_printf("stream: 0x%" PRIX32 " ('%.100s...')\n", stack[1], buf);
  char* fmt = (char*)Memory(stack[2]);
  info_printf("fmt: 0x%" PRIX32 " ('%s')\n", stack[2], fmt);

  // We'll let MS code handle buffer loads
  if (buf == NULL) {
    return;
  }

#if 0
  if (!strcmp(fmt, "%s")) {
    SetTracing(true);
  }
#endif

#if 0
  //FIXME: Hack.. for these to work, they'd have to consume the buffer
  if (!strcmp(fmt, "%s")) {
    eax = sscanf(buf, fmt, Memory(stack[3]));
    uc_reg_write(uc, UC_X86_REG_EAX, &eax);
    eip = returnAddress;
    uc_reg_write(uc, UC_X86_REG_EIP, &eip);
    esp += 4;
    uc_reg_write(uc, UC_X86_REG_ESP, &esp);
    return;
  }
  if (!strcmp(fmt, "%f %f %f")) {
    eax = 3; //sscanf(buf, fmt);
    uc_reg_write(uc, UC_X86_REG_EAX, &eax);
    eip = returnAddress;
    uc_reg_write(uc, UC_X86_REG_EIP, &eip);
    esp += 4;
    uc_reg_write(uc, UC_X86_REG_ESP, &esp);
    return;
  }
#endif


}





// This is strictly for debug purposes, it attempts to add messages to the log in case of the weird-crash
static void UcCrashHook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  int ecx;
  uc_reg_read(uc, UC_X86_REG_ECX, (void*)&ecx);
  int edi;
  uc_reg_read(uc, UC_X86_REG_EDI, (void*)&edi);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);

  char buf[1024];
  sprintf(buf, "Bug: ecx=0x%08" PRIX32 ", at eip=0x%08" PRIX32 ". 0x%08" PRIX32 " 0x%08" PRIX32 " 0x%08" PRIX32 " 0x%08" PRIX32,
          ecx, eip,
          stack[0], stack[1], stack[2], stack[3]);
  glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1, buf);
}


//Winmm.lib
HACKY_IMPORT_BEGIN(timeGetTime)
  //FIXME: Avoid overflow?
  eax = GetTicks();
HACKY_IMPORT_END()


// WTF.. why?! COMCTL ordinal import which does nothing
//Comctl32.lib
HACKY_IMPORT_BEGIN(InitCommonControls)
HACKY_IMPORT_END()


// CD check
//Mincore.lib
HACKY_IMPORT_BEGIN(GetFileVersionInfoSizeA)
  hacky_printf("lptstrFilename 0x%" PRIX32 " (%s)\n", stack[1], Memory(stack[1]));
  hacky_printf("lpdwHandle 0x%" PRIX32 "\n", stack[2]);

  if (stack[2]) {
    *(uint32_t*)Memory(stack[2]) = 0;
  }

  eax = 0; // Size of file version info [0 means error]
  esp += 2 * 4;
HACKY_IMPORT_END()



static void UcMallocHook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);

  // Pop the return address
  Address returnAddress = stack[0];
  info_printf("Return at 0x%" PRIX32 "\n", returnAddress);

  eax = Allocate(stack[1]);
  info_printf("malloc(%d) -> 0x%08X\n", stack[1], eax);

  uc_reg_write(uc, UC_X86_REG_EAX, (void*)&eax);
  eip = returnAddress;
  uc_reg_write(uc, UC_X86_REG_EIP, (void*)&eip);
  esp += 4;
  uc_reg_write(uc, UC_X86_REG_ESP, (void*)&esp);
}

static void UcFreeHook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);

  // Pop the return address
  Address returnAddress = stack[0];
  info_printf("Return at 0x%" PRIX32 "\n", returnAddress);

  info_printf("free(0x%08X)\n", stack[1]);
  Free(stack[1]);

  eax = 0;
  uc_reg_write(uc, UC_X86_REG_EAX, (void*)&eax);
  eip = returnAddress;
  uc_reg_write(uc, UC_X86_REG_EIP, (void*)&eip);
  esp += 4;
  uc_reg_write(uc, UC_X86_REG_ESP, (void*)&esp);
}


// Some TGA loading function

static void UcTGAHook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);

  // Pop the return address
  Address returnAddress = stack[0];
  info_printf("Return at 0x%" PRIX32 "\n", returnAddress);

  //int __cdecl sub_48A230(int a1, char *a2, _DWORD *a3, _DWORD *a4)
  info_printf("\n\n\n[ 48A230 ] TGAHook(0x%08X, 0x%08X, 0x%08X, 0x%08X)\n\n\n\n", stack[1], stack[2], stack[3], stack[4]);

  // Emulate instruction we overwrote
  eax = stack[1];
  uc_reg_write(uc, UC_X86_REG_EAX, (void*)&eax);
  eip = 0x48a234;
  uc_reg_write(uc, UC_X86_REG_EIP, (void*)&eip);
}



// Generic hook

// Callback for tracing instructions

void UnknownImport(uc_engine* uc, Address address, void* user_data) {
  int eip;
  uc_reg_read(uc, UC_X86_REG_EIP, (void*)&eip);
  int esp;
  uc_reg_read(uc, UC_X86_REG_ESP, (void*)&esp);
  int eax;
  uc_reg_read(uc, UC_X86_REG_EAX, (void*)&eax);
  
  Address stackAddress = esp;
  uint32_t* stack = (uint32_t*)Memory(stackAddress);

#if 0
  // This lists a stack trace.
  // It's a failed attempt because most functions omit the frame pointer
  int ebp;
  uc_reg_read(uc, UC_X86_REG_EBP, &ebp);
  StackTrace(ebp, 10, 4);
#endif

  // Pop the return address
  Address returnAddress = stack[0];
  eip = returnAddress;
  esp += 4;
  
  sys_printf("\nUnknown function!\n\n");

  sys_printf("Stack at 0x%" PRIX32 "; returning EAX: 0x%08" PRIX32 "\n", stackAddress, eax);
  sys_printf("%7" PRIu32 " Emulation at %X ('%s') from %X\n\n", callId, eip, (char*)user_data, returnAddress);

  callId++;

  assert(false);

  uc_reg_write(uc, UC_X86_REG_ESP, (void*)&esp);
  uc_reg_write(uc, UC_X86_REG_EIP, (void*)&eip);
  uc_reg_write(uc, UC_X86_REG_EAX, (void*)&eax);
}


void ExecuteGame(Game& game)
{
  game.Run();
}

int main(int argc, char* argv[]) {
  Application application;
  application.Init();
 
  StarWars game;
  game.Init(640, 480);
  
  std::thread thread_game(ExecuteGame, std::ref(game));
  application.Run(&game);
  game.set_request_stop(true);
  thread_game.join();
  
  game.Finish();
  
  application.Finish();
  
  return EXIT_SUCCESS;
}
