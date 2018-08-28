// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#ifndef __OPENSWE1R_EMULATION_H__
#define __OPENSWE1R_EMULATION_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define API(x) API__ ## x

extern FILE* handles[];
extern uint32_t handle_index;
extern uint32_t hevent_index;

//FIXME: use these..
typedef uint32_t Address;
typedef uint32_t Size;

typedef struct {
  uint32_t eip;
} Context;

typedef struct {
    uint8_t raw[10];
} X87Register;

typedef struct {
    
    uint64_t sleep;
    bool active;
    bool running;
    uint32_t id;
    
    // Standard stuff
    uint32_t eip;
    uint32_t esp;
    uint32_t ebp;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags;
    
    // x87
    uint16_t fpcw;
    uint16_t fpsw;
    uint16_t fptw;
    X87Register fp[8];
    
    //FIXME: MMX?
} ThreadContext;

void InitializeEmulation();
void CleanupEmulation();

void RunEmulation();

// Memory API

void MapMemory(void* data, uint32_t address, uint32_t size, bool read, bool write, bool execute);
Address Allocate(Size size);
void Free(Address address);
void* Memory(uint32_t address);

// Hook API

Address CreateHlt();
void AddHltHandler(Address address, void(*callback)(void* uc, Address address, void* user_data), void* user_data);
Address CreateCallback(void* callback, void* user);
Address CreateInt(uint32_t intno, uint32_t eax);
Address CreateInt21();

// Thread API
ThreadContext *GetCurrentThreadContext();
unsigned int CreateEmulatedThread(uint32_t eip, bool suspended);
void SleepThread(uint64_t duration);

// Debug API

void SetTracing(bool enabled);

#endif
