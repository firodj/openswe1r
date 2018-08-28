// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#ifndef __OPENSWE1R_MAIN_H__
#define __OPENSWE1R_MAIN_H__

#include <unicorn/unicorn.h>

#include "emulation.h"

typedef struct {
    const char* name;
    void(*callback)(void*, Address, void*);
    Address address;
    Address thunkAddress;
    int hook; // hook type, 1:HLT, 2:INT
} Export;

extern uint32_t callId;

extern const char** dirlisting;

extern Address clearEax;

extern uint32_t tls[];

static inline int LogPrintf(const char* fmt, ...) {
#if 1
  va_list args;
  va_start(args, fmt);
  int ret = vprintf(fmt, args);
  va_end(args);
  return ret;
#endif
}

static inline int LogSilent(const char* fmt, ...) { return 0;
}

#define hacky_printf LogSilent
#define info_printf LogSilent
#define sys_printf LogPrintf
#define my_printf LogPrintf

Address CreateInterface(const char* name, unsigned int slotCount, uint32_t objectSize);
void AddExport(const char* name, void* callback, Address address);
void AddExport2(const char* name, void* callback);
Export* LookupExportByName(const char* name);
char* TranslatePath(const char* path);

// Defines an INITIALIZER macro which will run code at startup
#if defined(__GNUC__)
#  define INITIALIZER(_name) \
    __attribute__((constructor)) void _name()
#elif defined(_MSC_VER)
#  if (_MSC_VER >= 1400)
#    if !defined(_WIN64)
#      pragma section(".CRT$XIU", read)
#    else
#      pragma section(".CRT$XIU", long, read)
#    endif
#    define INITIALIZER(_name) \
      void _name(); \
      static int __cdecl __ ## _name ## _caller() { _name(); return 0; } \
      __declspec(allocate(".CRT$XIU")) int(__cdecl *__ ## _name ## _pointer)() = __ ## _name ## _caller; \
      void _name()
#  else
#    error Compiler not supported
#  endif
#else
#  error Compiler not detected
#endif

#define HACKY_IMPORT_BEGIN(_name) \
  static void Hook_ ## _name (void* uc, Address _address, void* _user_data); \
  INITIALIZER(Register_ ## _name) { \
    const char* name = #_name; \
    info_printf("Registering hook for '%s'\n", name); \
    AddExport(name, Hook_ ## _name, 0); \
  } \
  static void Hook_ ## _name (void* uc, Address _address, void* _user_data) { \
    bool silent = false; \
    \
    int eip; \
    uc_reg_read(uc, UC_X86_REG_EIP, &eip); \
    int esp; \
    uc_reg_read(uc, UC_X86_REG_ESP, &esp); \
    int eax; \
    uc_reg_read(uc, UC_X86_REG_EAX, &eax); \
    \
    Address stackAddress = esp; \
    uint32_t* stack = (uint32_t*)Memory(stackAddress); \
    \
    /* This lists a stack trace. */ \
    /* It's a failed attempt because most functions omit the frame pointer */ \
    /*int ebp; \
    uc_reg_read(uc, UC_X86_REG_EBP, &ebp); \
    StackTrace(ebp, 10, 4); */ \
    \
    /* Pop the return address */ \
    Address returnAddress = stack[0]; \
    eip = returnAddress; \
    esp += 4; \

#define HACKY_IMPORT_END() \
    if (!silent) { \
      hacky_printf("Stack at 0x%" PRIX32 "; returning EAX: 0x%08" PRIX32 "\n", stackAddress, eax); \
      hacky_printf("%7" PRIu32 " Emulation at %X ('%s') from %X\n\n", callId, eip, (char*)_user_data, returnAddress); \
    } \
    callId++; \
    \
    uc_reg_write(uc, UC_X86_REG_ESP, &esp); \
    uc_reg_write(uc, UC_X86_REG_EIP, &eip); \
    uc_reg_write(uc, UC_X86_REG_EAX, &eax); \
  }

#define HACKY_COM_BEGIN(interface, slot) HACKY_IMPORT_BEGIN(interface ## __ ## slot)
#define HACKY_COM_END() HACKY_IMPORT_END()

#define HACKY_IMPORT_BEGIN2(_name) \
  static void Hook_ ## _name (void* uc, Address _address, void* _user_data); \
  INITIALIZER(Register_ ## _name) { \
    const char* name = #_name; \
    info_printf("Registering hook for '%s'\n", name); \
    AddExport2(name, Hook_ ## _name); \
  } \
  static void Hook_ ## _name (void* uc, Address _address, void* _user_data) { \
    int32_t eax, esp; \
    uc_reg_read(uc, UC_X86_REG_ESP, &esp); \
    uc_reg_read(uc, UC_X86_REG_EAX, &eax); \
    uint32_t* stack = (uint32_t*)Memory(esp); \
    Address returnAddress = stack[0]; \

#define HACKY_IMPORT_END2(nargs) \
    if (nargs) { \
       esp += nargs << 2; \
       uc_reg_write(uc, UC_X86_REG_ESP, &esp); \
       stack = (uint32_t*)Memory(esp); \
       stack[0] = returnAddress; \
    } \
    uc_reg_write(uc, UC_X86_REG_EAX, &eax); \
  }

#define HACKY_COM_BEGIN2(interface, slot) HACKY_IMPORT_BEGIN2(interface ## __ ## slot)
#define HACKY_COM_END2(nargs) HACKY_IMPORT_END2(nargs)

#endif
