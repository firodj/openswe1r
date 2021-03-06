// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "../main.h"
#include "../ms_windows.h"

#include <inttypes.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "al.h"
#include "alc.h"
#include "a3d.h"

typedef struct {
  void* vtable;
  ALCdevice *device;
  ALCcontext *context;
} A3d4;


typedef struct {
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
  uint16_t cbSize;
} API(WAVEFORMATEX);

typedef struct {
  void* vtable;
  ALuint al_source;
  ALuint al_buffer;
  API(WAVEFORMATEX) fmt;
  Address data;
  uint32_t size_data;
  uint32_t type;
} A3DSOURCE;


#define a3d_printf LogSilent

#if 0
DECLARE_INTERFACE_(IA3d4, IUnknown)
{
	// IUnknown Methods.
	STDMETHOD(QueryInterface)				(THIS_ REFIID, LPVOID FAR *) PURE; // 0
	STDMETHOD_(ULONG,AddRef)				(THIS) PURE; // 1
	STDMETHOD_(ULONG,Release)				(THIS) PURE; // 2

	// IA3d Methods.
	STDMETHOD(SetOutputMode)				(THIS_ DWORD, DWORD, DWORD) PURE; // 3
	STDMETHOD(GetOutputMode)				(THIS_ LPDWORD, LPDWORD, LPDWORD) PURE; // 4
	STDMETHOD(SetResourceManagerMode)		(THIS_ DWORD) PURE; // 5
	STDMETHOD(GetResourceManagerMode)		(THIS_ LPDWORD) PURE; // 6
	STDMETHOD(SetHFAbsorbFactor)			(THIS_ FLOAT) PURE; // 7
	STDMETHOD(GetHFAbsorbFactor)			(THIS_ FLOAT *) PURE; // 8

	// IA3d2 Methods.
	STDMETHOD(RegisterVersion)				(THIS_ DWORD) PURE; // 9
	STDMETHOD(GetSoftwareCaps)				(THIS_ LPA3DCAPS_SOFTWARE) PURE; // 10
	STDMETHOD(GetHardwareCaps)				(THIS_ LPA3DCAPS_HARDWARE) PURE; // 11

	// IA3d3 Methods.
	STDMETHOD(Clear)						(THIS) PURE; // 12
	STDMETHOD(Flush)						(THIS) PURE; // 13
	STDMETHOD(Compat)						(THIS_ DWORD, DWORD) PURE; // 14
	STDMETHOD(Init)							(THIS_ LPGUID, DWORD, DWORD) PURE; // 15
	STDMETHOD(IsFeatureAvailable)			(THIS_ DWORD) PURE; // 16
	STDMETHOD(NewSource)					(THIS_ DWORD, LPA3DSOURCE *) PURE; // 17
	STDMETHOD(DuplicateSource)				(THIS_ LPA3DSOURCE, LPA3DSOURCE *) PURE; // 18
	STDMETHOD(SetCooperativeLevel)			(THIS_ HWND, DWORD) PURE; // 19
	STDMETHOD(GetCooperativeLevel)			(THIS_ LPDWORD) PURE; // 20
	STDMETHOD(SetMaxReflectionDelayTime)	(THIS_ A3DVAL) PURE; // 21
	STDMETHOD(GetMaxReflectionDelayTime)	(THIS_ LPA3DVAL) PURE; // 22
	STDMETHOD(SetCoordinateSystem)			(THIS_ DWORD) PURE; // 23
	STDMETHOD(GetCoordinateSystem)			(THIS_ LPDWORD) PURE; // 24
	STDMETHOD(SetOutputGain)				(THIS_ A3DVAL) PURE; // 25
	STDMETHOD(GetOutputGain)				(THIS_ LPA3DVAL) PURE; // 26

	// IA3d4 Methods
	STDMETHOD(SetNumFallbackSources)		(THIS_ DWORD) PURE; // 27
	STDMETHOD(GetNumFallbackSources)		(THIS_ LPDWORD) PURE;	// 28
	STDMETHOD(SetRMPriorityBias)			(THIS_ A3DVAL) PURE; // 29
	STDMETHOD(GetRMPriorityBias)			(THIS_ LPA3DVAL) PURE; // 30
	STDMETHOD(DisableViewer)				(THIS) PURE; // 31
	STDMETHOD(SetUnitsPerMeter) 			(THIS_ A3DVAL) PURE; // 32
	STDMETHOD(GetUnitsPerMeter)				(THIS_ LPA3DVAL) PURE; // 33
	STDMETHOD(SetDopplerScale)				(THIS_ A3DVAL) PURE; // 34
	STDMETHOD(GetDopplerScale)				(THIS_ LPA3DVAL) PURE; // 35
	STDMETHOD(SetDistanceModelScale)		(THIS_ A3DVAL) PURE; // 36
	STDMETHOD(GetDistanceModelScale)		(THIS_ LPA3DVAL) PURE; // 37
	STDMETHOD(SetEq)						(THIS_ A3DVAL) PURE; // 38
	STDMETHOD(GetEq)						(THIS_ LPA3DVAL) PURE; // 39
	STDMETHOD(Shutdown)						(THIS) PURE; // 40
	STDMETHOD(RegisterApp)					(THIS_ REFIID) PURE; // 41
};

DECLARE_INTERFACE_(IA3dListener, IUnknown)
{
	// IUnknown Methods.
	STDMETHOD(QueryInterface)		(THIS_ REFIID, LPVOID FAR *) PURE; // 0
	STDMETHOD_(ULONG,AddRef)		(THIS) PURE; // 1
	STDMETHOD_(ULONG,Release)		(THIS) PURE; // 2

	// IA3dListener Methods.
	STDMETHOD(SetPosition3f)		(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 3
	STDMETHOD(GetPosition3f)		(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 4
	STDMETHOD(SetPosition3fv)		(THIS_ LPA3DVAL) PURE; // 5
	STDMETHOD(GetPosition3fv)		(THIS_ LPA3DVAL) PURE; // 6
	STDMETHOD(SetOrientationAngles3f)	(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 7
	STDMETHOD(GetOrientationAngles3f)	(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 8
	STDMETHOD(SetOrientationAngles3fv)	(THIS_ LPA3DVAL) PURE; // 9
	STDMETHOD(GetOrientationAngles3fv)	(THIS_ LPA3DVAL) PURE; // 10
	STDMETHOD(SetOrientation6f)		(THIS_ A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL) PURE; // 11
	STDMETHOD(GetOrientation6f)		(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 12
	STDMETHOD(SetOrientation6fv)	(THIS_ LPA3DVAL) PURE; // 13
	STDMETHOD(GetOrientation6fv)	(THIS_ LPA3DVAL) PURE; // 14
	STDMETHOD(SetVelocity3f)		(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 15
	STDMETHOD(GetVelocity3f)		(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 16
	STDMETHOD(SetVelocity3fv)		(THIS_ LPA3DVAL) PURE; // 17
	STDMETHOD(GetVelocity3fv)		(THIS_ LPA3DVAL) PURE; // 18
};

DECLARE_INTERFACE_(IA3dSource, IUnknown)
{
	// IUnknown Methods.
	STDMETHOD(QueryInterface)			(THIS_ REFIID, LPVOID FAR *) PURE; // 0
	STDMETHOD_(ULONG,AddRef)			(THIS) PURE; // 1
	STDMETHOD_(ULONG,Release)			(THIS) PURE; // 2

	// IA3dSource Methods.
	STDMETHOD(LoadWaveFile)				(THIS_ LPSTR) PURE; // 3
	STDMETHOD(LoadWaveData)				(THIS_ LPVOID, DWORD) PURE; // 4
	STDMETHOD(AllocateWaveData)			(THIS_ INT) PURE; // 5
	STDMETHOD(FreeWaveData)				(THIS) PURE; // 6
	STDMETHOD(SetWaveFormat)			(THIS_ LPVOID) PURE; // 7
	STDMETHOD(GetWaveFormat)			(THIS_ LPVOID) PURE; // 8
	STDMETHOD(GetWaveSize)				(THIS) PURE; // 9
	STDMETHOD(GetType)					(THIS_ LPDWORD) PURE; // 10
	STDMETHOD(Lock)						(THIS_ DWORD, DWORD, LPVOID *, LPDWORD, LPVOID *, LPDWORD, DWORD) PURE; // 11
	STDMETHOD(Unlock)					(THIS_ LPVOID, DWORD, LPVOID, DWORD) PURE; // 12
	STDMETHOD(Play)						(THIS_ INT) PURE; // 13
	STDMETHOD(Stop)						(THIS) PURE; // 14
	STDMETHOD(Rewind)					(THIS) PURE; // 15
	STDMETHOD(SetWaveTime)				(THIS_ A3DVAL) PURE; // 16
	STDMETHOD(GetWaveTime)				(THIS_ LPA3DVAL) PURE; // 17
	STDMETHOD(SetWavePosition)			(THIS_ DWORD) PURE; // 18
	STDMETHOD(GetWavePosition)			(THIS_ LPDWORD) PURE; // 19
	STDMETHOD(SetPosition3f)			(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 20
	STDMETHOD(GetPosition3f)			(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 21
	STDMETHOD(SetPosition3fv)			(THIS_ LPA3DVAL) PURE; // 22
	STDMETHOD(GetPosition3fv)			(THIS_ LPA3DVAL) PURE; // 23
	STDMETHOD(SetOrientationAngles3f)	(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 24
	STDMETHOD(GetOrientationAngles3f)	(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 25
	STDMETHOD(SetOrientationAngles3fv)	(THIS_ LPA3DVAL) PURE; // 26
	STDMETHOD(GetOrientationAngles3fv)	(THIS_ LPA3DVAL) PURE; // 27
 	STDMETHOD(SetOrientation6f)			(THIS_ A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL) PURE; // 28
	STDMETHOD(GetOrientation6f)			(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 29
	STDMETHOD(SetOrientation6fv)		(THIS_ LPA3DVAL) PURE; // 30
	STDMETHOD(GetOrientation6fv)		(THIS_ LPA3DVAL) PURE; // 31
	STDMETHOD(SetVelocity3f)			(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 32
	STDMETHOD(GetVelocity3f)			(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 33
	STDMETHOD(SetVelocity3fv)			(THIS_ LPA3DVAL) PURE; // 34
	STDMETHOD(GetVelocity3fv)			(THIS_ LPA3DVAL) PURE; // 35
	STDMETHOD(SetCone)					(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 36
	STDMETHOD(GetCone)					(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 37
	STDMETHOD(SetMinMaxDistance)		(THIS_ A3DVAL, A3DVAL, DWORD) PURE; // 38
	STDMETHOD(GetMinMaxDistance)		(THIS_ LPA3DVAL, LPA3DVAL, LPDWORD) PURE; // 39
	STDMETHOD(SetGain)					(THIS_ A3DVAL) PURE; // 40
	STDMETHOD(GetGain)					(THIS_ LPA3DVAL) PURE; // 41
	STDMETHOD(SetPitch)					(THIS_ A3DVAL) PURE; // 42
	STDMETHOD(GetPitch)					(THIS_ LPA3DVAL) PURE; // 43
	STDMETHOD(SetDopplerScale)			(THIS_ A3DVAL) PURE; // 44
	STDMETHOD(GetDopplerScale)			(THIS_ LPA3DVAL) PURE; // 45
	STDMETHOD(SetDistanceModelScale)	(THIS_ A3DVAL) PURE; // 46
	STDMETHOD(GetDistanceModelScale)	(THIS_ LPA3DVAL) PURE; // 47
	STDMETHOD(SetEq)					(THIS_ A3DVAL) PURE; // 48
	STDMETHOD(GetEq)					(THIS_ LPA3DVAL) PURE; // 49
	STDMETHOD(SetPriority)				(THIS_ A3DVAL) PURE; // 50
	STDMETHOD(GetPriority)				(THIS_ LPA3DVAL) PURE; // 51
	STDMETHOD(SetRenderMode)			(THIS_ DWORD) PURE; // 52
	STDMETHOD(GetRenderMode)			(THIS_ LPDWORD) PURE; // 53
	STDMETHOD(GetAudibility)			(THIS_ LPA3DVAL) PURE; // 54
	STDMETHOD(GetOcclusionFactor)		(THIS_ LPA3DVAL) PURE; // 55
    STDMETHOD(GetStatus)				(THIS_ LPDWORD) PURE; // 56
    STDMETHOD(SetPanValues)				(THIS_ DWORD, LPA3DVAL) PURE; // 57
    STDMETHOD(GetPanValues)				(THIS_ DWORD, LPA3DVAL) PURE; // 58
	STDMETHOD(SetWaveEvent)				(THIS_ DWORD, HANDLE) PURE; // 59
	STDMETHOD(ClearWaveEvents)			(THIS) PURE; // 60
	STDMETHOD(SetTransformMode)			(THIS_ DWORD) PURE; // 61
	STDMETHOD(GetTransformMode)			(THIS_ LPDWORD) PURE; // 62
	STDMETHOD(SetReflectionDelayScale)	(THIS_ A3DVAL) PURE; // 63
	STDMETHOD(GetReflectionDelayScale)	(THIS_ LPA3DVAL) PURE; // 64
	STDMETHOD(SetReflectionGainScale)	(THIS_ A3DVAL) PURE; // 65
	STDMETHOD(GetReflectionGainScale)	(THIS_ LPA3DVAL) PURE; // 66
};


#endif


// Implementation

// IA3d4 -> STDMETHOD(QueryInterface)				(THIS_ REFIID, LPVOID FAR *) PURE; // 0
HACKY_COM_BEGIN(IA3d4, 0)
  a3d_printf("IA3d4::QueryInterface\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);

  const API(IID)* iid = (const API(IID)*)Memory(stack[2]);

  char iidString[1024];
  sprintf(iidString, "%08" PRIX32 "-%04" PRIX16 "-%04" PRIX16 "-%02" PRIX8 "%02" PRIX8 "-%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8,
          iid->Data1, iid->Data2, iid->Data3,
          iid->Data4[0], iid->Data4[1], iid->Data4[2], iid->Data4[3],
          iid->Data4[4], iid->Data4[5], iid->Data4[6], iid->Data4[7]);
  info_printf("  (read iid: {%s})\n", iidString);

  char name[32];
  //FIXME: Add more classed / interfaces

  if (!strcmp(iidString, "C398E563-D90B-11D1-90FB-006008A1F441")) {
    strcpy(name, "IA3dListener");
  } else {
    assert(false);
  }
  
  *(Address*)Memory(stack[3]) = CreateInterface(name, 200, 100);

  eax = 0;
  esp += 3 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD_(ULONG,Release)       (THIS) PURE; //2
HACKY_COM_BEGIN(IA3d4, 2)
  a3d_printf("IA3d4::Release\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(GetHardwareCaps)				(THIS_ LPA3DCAPS_HARDWARE) PURE; // 11
HACKY_COM_BEGIN(IA3d4, 11)
  a3d_printf("IA3d4::GetHardwareCaps\n");
  hacky_printf("GetHardwareCaps\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(Flush)						(THIS) PURE; // 13
HACKY_COM_BEGIN(IA3d4, 13)
  a3d_printf("IA3d4::Flush\n");
  hacky_printf("Flush\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0;
  esp += 1 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(Init)							(THIS_ LPGUID, DWORD, DWORD) PURE; // 15
HACKY_COM_BEGIN(IA3d4, 15)
  a3d_printf("IA3d4::Init\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);

  A3d4* This = (A3d4*)Memory(stack[1]);

  This->device = alcOpenDevice(NULL);
  assert(This->device != NULL);

  This->context = alcCreateContext(This->device, NULL);

  //FIXME: Do this on every call which uses this context instead
  if (!alcMakeContextCurrent(This->context)) {
    assert(false);
  }

  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(NewSource)					(THIS_ DWORD, LPA3DSOURCE *) PURE; // 17
HACKY_COM_BEGIN2(IA3d4, 17)
  a3d_printf("IA3d4::NewSource");
  a3d_printf(" this:0x%" PRIX32, stack[1]);
  a3d_printf(" dwFlags:0x%" PRIX32, stack[2]);

  Address addr = CreateInterface("IA3dSource", 200, sizeof(A3DSOURCE));
  
  A3DSOURCE* source = (A3DSOURCE*)Memory(addr);
  alGenSources(1, &source->al_source);
  alGenBuffers(1, &source->al_buffer);
  source->type = stack[2];
  source->size_data = 0;
  source->data = 0;

  //FIXME: Move these to proper functions, unless we need defaults
  if (source->type & A3DSOURCE_TYPESTREAMED) {
    alSourcei(source->al_source, AL_SOURCE_TYPE, AL_STREAMING);
  }

  alSourcef(source->al_source, AL_PITCH, 1);
  // check for errors
  alSourcef(source->al_source, AL_GAIN, 1);
  // check for errors
  alSource3f(source->al_source, AL_POSITION, 0, 0, 0);
  // check for errors
  alSource3f(source->al_source, AL_VELOCITY, 0, 0, 0);
  // check for errors
  alSourcei(source->al_source, AL_LOOPING, AL_FALSE);

  a3d_printf(" ppSource:=0x%" PRIX32 "\n", addr);
  *(Address*)Memory(stack[3]) = addr;  

  eax = 0;
  //esp += 3 * 4;
HACKY_COM_END2(3)

// IA3d4 -> STDMETHOD(DuplicateSource)                (THIS_ LPA3DSOURCE, LPA3DSOURCE *) PURE; // 18
HACKY_COM_BEGIN2(IA3d4, 18)
  a3d_printf("IA3d4::DuplicateSource");
  a3d_printf(" this:0x%" PRIX32, stack[1]);
  a3d_printf(" pOriginal:0x%" PRIX32, stack[2]);

  A3DSOURCE* source = (A3DSOURCE*)Memory(stack[2]);
  Address addr = CreateInterface("IA3dSource", 200, sizeof(A3DSOURCE));
  A3DSOURCE* dest = (A3DSOURCE*)Memory(addr);
  alGenSources(1, &dest->al_source);
  alGenBuffers(1, &dest->al_buffer);
  dest->type = source->type;
  dest->fmt = source->fmt;

  //FIXME: should wave data also duplicated?
  dest->data = 0;
  dest->size_data = 0;

  a3d_printf(" ppCopy:=0x%" PRIX32 "\n", addr);
  *(Address*)Memory(stack[3]) = addr;

  eax = 0;
  //esp += 3 * 4;
HACKY_COM_END2(3)

// IA3d4 -> STDMETHOD(SetCooperativeLevel)			(THIS_ HWND, DWORD) PURE; // 19
HACKY_COM_BEGIN(IA3d4, 19)
  a3d_printf("IA3d4::SetCooperativeLevel\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  eax = 0;
  esp += 3 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetCoordinateSystem)			(THIS_ DWORD) PURE; // 23
HACKY_COM_BEGIN(IA3d4, 23)
  a3d_printf("IA3d4::SetCoordinateSystem\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetOutputGain)				(THIS_ A3DVAL) PURE; // 25
HACKY_COM_BEGIN(IA3d4, 25)
  a3d_printf("IA3d4::SetOutputGain\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(GetOutputGain)				(THIS_ LPA3DVAL) PURE; // 26
HACKY_COM_BEGIN(IA3d4, 26)
  a3d_printf("IA3d4::GetOutputGain\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetNumFallbackSources)		(THIS_ DWORD) PURE; // 27
HACKY_COM_BEGIN(IA3d4, 27)
  a3d_printf("IA3d4::SetNumFallbackSources\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetUnitsPerMeter) 			(THIS_ A3DVAL) PURE; // 32
HACKY_COM_BEGIN(IA3d4, 32)
  a3d_printf("IA3d4::SetUnitsPerMeter\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetDopplerScale)				(THIS_ A3DVAL) PURE; // 34
HACKY_COM_BEGIN(IA3d4, 34)
  a3d_printf("IA3d4::SetDopplerScale\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetDistanceModelScale)		(THIS_ A3DVAL) PURE; // 36
HACKY_COM_BEGIN(IA3d4, 36)
  a3d_printf("IA3d4::SetDistanceModelScale\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetEq)						(THIS_ A3DVAL) PURE; // 38
HACKY_COM_BEGIN(IA3d4, 38)
  a3d_printf("IA3d4::SetEq\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD_(ULONG,Release)			(THIS) PURE; // 2
HACKY_COM_BEGIN2(IA3dSource, 2)
  a3d_printf("IA3dSource::Release\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0;
  //esp += 1 * 4;
HACKY_COM_END2(1)

// IA3dSource -> STDMETHOD(AllocateWaveData)			(THIS_ INT) PURE; // 5
// Ref: AllocateAudioData
HACKY_COM_BEGIN2(IA3dSource, 5)
  a3d_printf("IA3dSource::AllocateWaveData");
  a3d_printf(" this:0x%" PRIX32, stack[1]);
  a3d_printf(" nSize:0x%" PRIX32 "\n", stack[2]);

  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
  This->size_data = stack[2];
  This->data = Allocate(This->size_data);

  eax = 0;
HACKY_COM_END2(2)

// IA3dSource -> STDMETHOD(SetWaveFormat)			(THIS_ LPVOID) PURE; // 7
// Ref: SetAudioFormat
HACKY_COM_BEGIN2(IA3dSource, 7)
  a3d_printf("IA3dSource::SetWaveFormat");
  a3d_printf(" this:0x%" PRIX32, stack[1]);
  a3d_printf(" pWaveFormat:0x%" PRIX32 "\n", stack[2]);

  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
  memcpy(&This->fmt, Memory(stack[2]), sizeof(API(WAVEFORMATEX)));

  eax = 0;
  //esp += 2 * 4;
HACKY_COM_END2(2)

// IA3dSource -> STDMETHOD(GetType)					(THIS_ LPDWORD) PURE; // 10
HACKY_COM_BEGIN2(IA3dSource, 10)
  a3d_printf("IA3dSource::GetType");
  a3d_printf(" this:0x%" PRIX32, stack[1]);

  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);

  a3d_printf(" dwType:=0x%" PRIX32 "\n", This->type);
  *(uint32_t*)Memory(stack[2]) = This->type;

  eax = 0;
  //esp += 2 * 4;
HACKY_COM_END2(2)

// IA3dSource -> STDMETHOD(Lock)						(THIS_ DWORD, DWORD, LPVOID *, LPDWORD, LPVOID *, LPDWORD, DWORD) PURE; // 11
HACKY_COM_BEGIN2(IA3dSource, 11)
  a3d_printf("Ia3dSource::Lock");
  a3d_printf(" this:0x%" PRIX32, stack[1]);
  a3d_printf(" dwWriteCursor:0x%" PRIX32, stack[2]);
  a3d_printf(" dwNumBytes:0x%" PRIX32, stack[3]);

  assert(stack[8] == 0);

  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
  uint32_t dwWriteCursor = stack[2];

  //FIXME: assert that the requested length is shorter than the buffer len etc.
  Address pvAudioPtr1 = This->data + dwWriteCursor;
  uint32_t dwAudioBytes1 = stack[3];
  a3d_printf(" pvAudioPtr1:=0x%" PRIX32, pvAudioPtr1);
  *(Address*)Memory(stack[4]) = pvAudioPtr1;
  a3d_printf(" dwAudioBytes1:=0x%" PRIX32, dwAudioBytes1);
  *(uint32_t*)Memory(stack[5]) = dwAudioBytes1;

  // Check if we can write a second buffer
  a3d_printf(" pvAudioPtr2:0x%" PRIX32, stack[6]);
  a3d_printf(" dwAudioBytes2:0x%" PRIX32, stack[7]);

  if (stack[6] && stack[7]) {
    *(Address*)Memory(stack[6]) = 0;
    *(uint32_t*)Memory(stack[7]) = 0;
  }
  a3d_printf(" dwFlags:0x%" PRIX32 "\n", stack[8]);

  eax = 0;
  //esp += 8 * 4;
HACKY_COM_END2(8)

// IA3dSource -> STDMETHOD(Unlock)					(THIS_ LPVOID, DWORD, LPVOID, DWORD) PURE; // 12
HACKY_COM_BEGIN2(IA3dSource, 12)
  a3d_printf("IA3dSource::Unlock");
  a3d_printf(" this:0x%" PRIX32, stack[1]);
  a3d_printf(" pvAudioPtr1:0x%" PRIX32, stack[2]);
  a3d_printf(" dwNumBytes1:0x%" PRIX32, stack[3]);
  a3d_printf(" pvAudioPtr2:0x%" PRIX32, stack[4]);
  a3d_printf(" dwNumBytes2:0x%" PRIX32 "\n", stack[5]);

  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
  ALenum al_format; 
  if (This->fmt.nChannels == 1) {
    if (This->fmt.wBitsPerSample == 8) {
      al_format = AL_FORMAT_MONO8;
    } else if (This->fmt.wBitsPerSample == 16) {
      al_format = AL_FORMAT_MONO16;
    } else {
      assert(false);
    }
  } else if (This->fmt.nChannels == 2) {
    if (This->fmt.wBitsPerSample == 8) {
      al_format = AL_FORMAT_STEREO8;
    } else if (This->fmt.wBitsPerSample == 16) {
      al_format = AL_FORMAT_STEREO16;
    } else {
      assert(false);
    }
  } else {
    assert(false);
  }

  assert(This->fmt.wFormatTag == 0x0001);
  assert(This->fmt.nBlockAlign == (This->fmt.nChannels * This->fmt.wBitsPerSample / 8));

  Address pvAudioPtr1 = stack[2];
  uint32_t dwNumBytes1 = stack[3];
  assert(pvAudioPtr1 >= This->data && pvAudioPtr1 < This->data + This->size_data);

  //alBufferData(This->al_buffer, al_format, Memory(pvAudioPtr1), dwNumBytes1, This->fmt.nSamplesPerSec);
  alBufferData(This->al_buffer, al_format, Memory(This->data), This->size_data, This->fmt.nSamplesPerSec);

  //FIXME: assert that this source isn't already playing etc.
  ALint status;
  alGetSourcei(This->al_source, AL_SOURCE_STATE, &status);
  //if (status != AL_PLAYING) {
    alSourcei(This->al_source, AL_BUFFER, This->al_buffer);
  //}
  assert(stack[4] == 0);
  assert(stack[5] == 0);

  eax = 0;
  //esp += 5 * 4;
HACKY_COM_END2(5)

// IA3dSource -> STDMETHOD(Play)						(THIS_ INT) PURE; // 13
HACKY_COM_BEGIN2(IA3dSource, 13)
  a3d_printf("IA3dSource::Play");
  a3d_printf(" this:0x%" PRIX32, stack[1]);
  a3d_printf(" nMode:0x%" PRIX32 "\n", stack[2]);

  bool looped = stack[2] == A3D_LOOPED;

  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
  ALint status;
  //alGetSourcei(This->al_source, AL_SOURCE_STATE, &status);
  //if (status != AL_PLAYING) {
    alSourcei(This->al_source, AL_LOOPING, looped);
    alSourcePlay(This->al_source);
  //}
  eax = 0;
  //esp += 2 * 4;
HACKY_COM_END2(2)

// IA3dSource -> STDMETHOD(Stop)						(THIS) PURE; // 14
HACKY_COM_BEGIN2(IA3dSource, 14)
  a3d_printf("IA3dSource::Stop");
  a3d_printf(" this:0x%" PRIX32 "\n", stack[1]);

  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
  alSourceStop(This->al_source);
  eax = 0;
  //esp += 1 * 4;
HACKY_COM_END2(1)

// IA3dSource -> STDMETHOD(Rewind)					(THIS) PURE; // 15
HACKY_COM_BEGIN2(IA3dSource, 15)
  a3d_printf("IA3dSource::Rewind");
  a3d_printf(" this:0x%" PRIX32 "\n", stack[1]);

  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
  alSourcei(This->al_source, AL_BYTE_OFFSET, 0);
  eax = 0;
  //esp += 1 * 4;
HACKY_COM_END2(1)

// IA3dSource -> STDMETHOD(SetWavePosition)            (THIS_ DWORD) PURE; // 18
HACKY_COM_BEGIN(IA3dSource, 18)
    a3d_printf("IA3dSource::SetWavePosition\n");
    hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
    hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

    A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
    alSourcei(This->al_source, AL_BYTE_OFFSET, stack[2]);

    eax = 0;
    esp += 2 << 2;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(GetWavePosition)            (THIS_ LPDWORD) PURE; // 19
// Ref: GetPlayPosition
HACKY_COM_BEGIN2(IA3dSource, 19)
    a3d_printf("IA3dSource::GetWavePosition");
    a3d_printf(" this:0x%" PRIX32, stack[1]);

    A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
    ALint pos;
    alGetSourcei(This->al_source, AL_BYTE_OFFSET, &pos); // AL_BYTE_OFFSET or AL_SAMPLE_OFFSET ?

    a3d_printf(" dwOffset:=0x%" PRIX32 "\n", pos);
    *(uint32_t*)Memory(stack[2]) = pos;

    eax = 0;
    //esp += 2 << 2;
HACKY_COM_END2(2)

// IA3dSource -> STDMETHOD(SetPosition3f)			(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 20
HACKY_COM_BEGIN(IA3dSource, 20)
  a3d_printf("IA3dSource::SetPosition3f\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetVelocity3f)			(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 32
HACKY_COM_BEGIN(IA3dSource, 32)
  a3d_printf("IA3dSource::SetVelocity3f\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetMinMaxDistance)		(THIS_ A3DVAL, A3DVAL, DWORD) PURE; // 38
HACKY_COM_BEGIN(IA3dSource, 38)
  a3d_printf("IA3dSource::SetMinMaxDistance\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetGain)					(THIS_ A3DVAL) PURE; // 40
HACKY_COM_BEGIN2(IA3dSource, 40)
  a3d_printf("IA3dSource::SetGain");
  a3d_printf(" this:0x%" PRIX32, stack[1]);

  float fval = *(float*)&stack[2];
  a3d_printf(" fGain:%f\n", fval);

  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
  alSourcef(This->al_source, AL_GAIN, fval);

  eax = 0;
  //esp += 2 * 4;
HACKY_COM_END2(2)

// IA3dSource -> STDMETHOD(SetPitch)					(THIS_ A3DVAL) PURE; // 42
HACKY_COM_BEGIN2(IA3dSource, 42)
  a3d_printf("IA3dSource::SetPitch");
  a3d_printf(" this:0x%" PRIX32, stack[1]);

  float fval = *(float*)&stack[2];
  a3d_printf(" fPitch:%f\n", fval);

  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
  alSourcef(This->al_source, AL_PITCH, fval);

  eax = 0;
  //esp += 2 * 4;
HACKY_COM_END2(2)

// IA3dSource -> STDMETHOD(SetDopplerScale)			(THIS_ A3DVAL) PURE; // 44
HACKY_COM_BEGIN(IA3dSource, 44)
  a3d_printf("IA3dSource::SetDopplerScale\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetRenderMode)			(THIS_ DWORD) PURE; // 52
HACKY_COM_BEGIN2(IA3dSource, 52)
  a3d_printf("IA3dSource::SetRenderMode");
  a3d_printf(" this:0x%" PRIX32, stack[1]);
  a3d_printf(" dwMode:0x%" PRIX32 "\n", stack[2]);
  eax = 0;

  //esp += 2 * 4;
HACKY_COM_END2(2)

// IA3dSource -> STDMETHOD(GetRenderMode)			(THIS_ LPDWORD) PURE; // 53
HACKY_COM_BEGIN(IA3dSource, 53)
  a3d_printf("IA3dSource::GetRenderMode\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

  *(uint32_t*)Memory(stack[2]) = 0;

  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(GetStatus)				(THIS_ LPDWORD) PURE; // 56
HACKY_COM_BEGIN2(IA3dSource, 56)
  a3d_printf("IA3dSource::GetStatus");
  a3d_printf(" this:0x%" PRIX32, stack[1]);

  uint32_t dwStatus = 0;
  A3DSOURCE* This = (A3DSOURCE*)Memory(stack[1]);
  ALint status;
  alGetSourcei(This->al_source, AL_SOURCE_STATE, &status);
  if (status == AL_PLAYING) dwStatus |= A3DSTATUS_PLAYING;
  alGetSourcei(This->al_source, AL_LOOPING, &status);
  if (status == AL_TRUE) dwStatus |= A3DSTATUS_LOOPING;

  a3d_printf("dwStatus:=0x%" PRIX32 "\n", dwStatus);
  *(uint32_t*)Memory(stack[2]) = dwStatus;

  eax = 0;
HACKY_COM_END2(2)

// IA3dSource -> STDMETHOD(SetPanValues)				(THIS_ DWORD, LPA3DVAL) PURE; // 57
HACKY_COM_BEGIN(IA3dSource, 57)
  a3d_printf("IA3dSource::SetPanValues\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("nChannels 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("fGains 0x%" PRIX32 "\n", stack[3]);

  eax = 0;
  esp += 3 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetWaveEvent)				(THIS_ DWORD, HANDLE) PURE; // 59
// Ref: SetPlayEvent
HACKY_COM_BEGIN2(IA3dSource, 59)
  a3d_printf("IA3dSource::SetWaveEvent");
  a3d_printf(" this:0x%" PRIX32, stack[1]);
  a3d_printf(" dwOffset:0x%" PRIX32, stack[2]);
  a3d_printf(" hEvent:0x%" PRIX32 "\n", stack[3]);
  eax = 0;
  //esp += 3 * 4;
HACKY_COM_END2(3)

// IA3dSource -> STDMETHOD(SetTransformMode)			(THIS_ DWORD) PURE; // 61
HACKY_COM_BEGIN(IA3dSource, 61)
  a3d_printf("IA3dSource::SetTransformMode\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()



// IA3dListener -> STDMETHOD_(ULONG,Release)       (THIS) PURE; //2
HACKY_COM_BEGIN(IA3dListener, 2)
  a3d_printf("IA3dListener::Release\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IA3dListener -> STDMETHOD(SetPosition3f)		(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 3
HACKY_COM_BEGIN(IA3dListener, 3)
  a3d_printf("IA3dListener::SetPosition3f\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

// IA3dListener -> STDMETHOD(SetOrientation6f)		(THIS_ A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL) PURE; // 11
HACKY_COM_BEGIN(IA3dListener, 11)
  a3d_printf("IA3dListener::SetOrientation6f\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("d 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("e 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("f 0x%" PRIX32 "\n", stack[7]);
  eax = 0;
  esp += 7 * 4;
HACKY_COM_END()

// IA3dListener -> STDMETHOD(SetVelocity3f)		(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 15
HACKY_COM_BEGIN(IA3dListener, 15)
  a3d_printf("IA3dListener::SetVelocity3f\n");

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

