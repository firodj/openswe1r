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

static bool ticks_started = false;

#if defined(__APPLE__)

#include <mach/mach_time.h>
static uint64_t start_mach;
mach_timebase_info_data_t mach_base_info;

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

#ifdef _WIN32
#include <mmsystem.h>
#pragma comment( lib, "Winmm.lib" )

/* The first (low-resolution) ticks value of the application */
static DWORD start = 0;

/* Store if a high-resolution performance counter exists on the system */
static bool hires_timer_available;
/* The first high-resolution ticks value of the application */
static LARGE_INTEGER hires_start_ticks;
/* The number of ticks per second of the high-resolution performance counter */
static LARGE_INTEGER hires_ticks_per_second;


void TicksInit(void)
{
	if (ticks_started) {
		return;
	}
	ticks_started = true;

	/* Set first ticks value */
	/* QueryPerformanceCounter has had problems in the past, but lots of games
	   use it, so we'll rely on it here.
	 */
	if (QueryPerformanceFrequency(&hires_ticks_per_second) == TRUE) {
		hires_timer_available = true;
		QueryPerformanceCounter(&hires_start_ticks);
	}
	else {
		hires_timer_available = false;
#ifndef __WINRT__
		start = timeGetTime();
#endif /* __WINRT__ */
	}
}

uint32_t GetTicks(void)
{
	DWORD now = 0;
	LARGE_INTEGER hires_now;

	if (!ticks_started) {
		TicksInit();
	}

	if (hires_timer_available) {
		QueryPerformanceCounter(&hires_now);

		hires_now.QuadPart -= hires_start_ticks.QuadPart;
		hires_now.QuadPart *= 1000;
		hires_now.QuadPart /= hires_ticks_per_second.QuadPart;

		return (DWORD)hires_now.QuadPart;
	}
	else {
#ifndef __WINRT__
		now = timeGetTime();
#endif /* __WINRT__ */
	}

	return (now - start);
}

bool GetPerformanceCounter(uint64_t* lpPerformanceCount)
{
	LARGE_INTEGER counter;

	if (!QueryPerformanceCounter(&counter)) {
		*lpPerformanceCount = GetTicks();
	} else {
		*lpPerformanceCount = counter.QuadPart;
	}
	return *lpPerformanceCount;
}

bool GetPerformanceFrequency(uint64_t* lpFrequency)
{
	LARGE_INTEGER frequency;

	if (!QueryPerformanceFrequency(&frequency)) {
		*lpFrequency = 1000;
	} else {
		*lpFrequency = frequency.QuadPart;
	}
	return *lpFrequency;
}

#endif