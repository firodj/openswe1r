#include "main.h"
#ifndef _WIN32
  #include "../ms_windows.h"
#else
  #include <windows.h>
#endif
#include "export.h"
#include "emulation.h"
#include <string.h>
#include <assert.h>
#include "common.h"

#if defined(__APPLE__)

#include <mach/mach_time.h>
static uint64_t start_mach;
mach_timebase_info_data_t mach_base_info;
static bool ticks_started = false;
bool has_monotonic_time = false;

void TicksInit(void)
{
    if (ticks_started) return;
    ticks_started = true;
    
    kern_return_t ret = mach_timebase_info(&mach_base_info);
    assert(ret == 0);
    has_monotonic_time = true;
}

void TicksQuit(void)
{
    ticks_started = false;
}

uint32_t GetTicks(void)
{
    uint32_t ticks;
    if (!ticks_started) TicksInit();
    
    assert(has_monotonic_time);
    uint64_t now = mach_absolute_time();
    ticks = (uint32_t)((((now - start_mach) * mach_base_info.numer) / mach_base_info.denom) / 1000000);

    return (ticks);
}

bool GetPerformanceCounter(uint64_t* lpPerformanceCount)
{
    uint64_t ticks;
    if (!ticks_started) TicksInit();
    
    assert(has_monotonic_time);
    ticks = mach_absolute_time();

    *lpPerformanceCount = ticks;
    return (ticks);
}

bool GetPerformanceFrequency(uint64_t* lpFrequency)
{
    if (!ticks_started) TicksInit();
    
    assert (has_monotonic_time);
    uint64_t freq = mach_base_info.denom;
    
    freq *= 1000000000;
    freq /= mach_base_info.numer;
    
    *lpFrequency = freq;

    return true;
}

#endif
