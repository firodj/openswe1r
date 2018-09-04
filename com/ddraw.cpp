// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "../main.h"
#include "../windows.h"
#include "../export.h"
#include "../emulation.h"
#include "glad/glad.h"
#include "ddraw.h"
#include <string.h>
#include <assert.h>

#include "../Application.hpp"
#include "../Game.hpp"

#if 0


DECLARE_INTERFACE_( IDirectDraw4, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE; // 0
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE; // 1
    STDMETHOD_(ULONG,Release) (THIS) PURE; // 2
    /*** IDirectDraw methods ***/
    STDMETHOD(Compact)(THIS) PURE; // 3
    STDMETHOD(CreateClipper)(THIS_ DWORD, LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR * ) PURE; // 4
    STDMETHOD(CreatePalette)(THIS_ DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR * ) PURE; // 5
    STDMETHOD(CreateSurface)(THIS_  LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE4 FAR *, IUnknown FAR *) PURE; // 6
    STDMETHOD(DuplicateSurface)( THIS_ LPDIRECTDRAWSURFACE4, LPDIRECTDRAWSURFACE4 FAR * ) PURE;
    STDMETHOD(EnumDisplayModes)( THIS_ DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2 ) PURE; // 8
    STDMETHOD(EnumSurfaces)(THIS_ DWORD, LPDDSURFACEDESC2, LPVOID,LPDDENUMSURFACESCALLBACK2 ) PURE;
    STDMETHOD(FlipToGDISurface)(THIS) PURE; // 10
    STDMETHOD(GetCaps)( THIS_ LPDDCAPS, LPDDCAPS) PURE;
    STDMETHOD(GetDisplayMode)( THIS_ LPDDSURFACEDESC2) PURE; //12
    STDMETHOD(GetFourCCCodes)(THIS_  LPDWORD, LPDWORD ) PURE;
    STDMETHOD(GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE4 FAR *) PURE; // 14
    STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetScanLine)(THIS_ LPDWORD) PURE; // 16
    STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL ) PURE;
    STDMETHOD(Initialize)(THIS_ GUID FAR *) PURE; // 18
    STDMETHOD(RestoreDisplayMode)(THIS) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD) PURE; // 20
    STDMETHOD(SetDisplayMode)(THIS_ DWORD, DWORD,DWORD, DWORD, DWORD) PURE;
    STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD, HANDLE ) PURE; // 22
    /*** Added in the v2 interface ***/
    STDMETHOD(GetAvailableVidMem)(THIS_ LPDDSCAPS2, LPDWORD, LPDWORD) PURE; // 23
    /*** Added in the V4 Interface ***/
    STDMETHOD(GetSurfaceFromDC) (THIS_ HDC, LPDIRECTDRAWSURFACE4 *) PURE; // 24
    STDMETHOD(RestoreAllSurfaces)(THIS) PURE;
    STDMETHOD(TestCooperativeLevel)(THIS) PURE; // 26
    STDMETHOD(GetDeviceIdentifier)(THIS_ LPDDDEVICEIDENTIFIER, DWORD ) PURE;
};


DECLARE_INTERFACE_( IDirectDrawSurface4, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE; // 0
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE; // 2
    /*** IDirectDrawSurface methods ***/
    STDMETHOD(AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE4) PURE; // 3
    STDMETHOD(AddOverlayDirtyRect)(THIS_ LPRECT) PURE; // 4
    STDMETHOD(Blt)(THIS_ LPRECT,LPDIRECTDRAWSURFACE4, LPRECT,DWORD, LPDDBLTFX) PURE;
    STDMETHOD(BltBatch)(THIS_ LPDDBLTBATCH, DWORD, DWORD ) PURE; // 6
    STDMETHOD(BltFast)(THIS_ DWORD,DWORD,LPDIRECTDRAWSURFACE4, LPRECT,DWORD) PURE;
    STDMETHOD(DeleteAttachedSurface)(THIS_ DWORD,LPDIRECTDRAWSURFACE4) PURE; // 8
    STDMETHOD(EnumAttachedSurfaces)(THIS_ LPVOID,LPDDENUMSURFACESCALLBACK2) PURE;
    STDMETHOD(EnumOverlayZOrders)(THIS_ DWORD,LPVOID,LPDDENUMSURFACESCALLBACK2) PURE; // 10
    STDMETHOD(Flip)(THIS_ LPDIRECTDRAWSURFACE4, DWORD) PURE;
    STDMETHOD(GetAttachedSurface)(THIS_ LPDDSCAPS2, LPDIRECTDRAWSURFACE4 FAR *) PURE; // 12
    STDMETHOD(GetBltStatus)(THIS_ DWORD) PURE;
    STDMETHOD(GetCaps)(THIS_ LPDDSCAPS2) PURE; // 14
    STDMETHOD(GetClipper)(THIS_ LPDIRECTDRAWCLIPPER FAR*) PURE;
    STDMETHOD(GetColorKey)(THIS_ DWORD, LPDDCOLORKEY) PURE; // 16
    STDMETHOD(GetDC)(THIS_ HDC FAR *) PURE;
    STDMETHOD(GetFlipStatus)(THIS_ DWORD) PURE; // 18
    STDMETHOD(GetOverlayPosition)(THIS_ LPLONG, LPLONG ) PURE;
    STDMETHOD(GetPalette)(THIS_ LPDIRECTDRAWPALETTE FAR*) PURE; // 20
    STDMETHOD(GetPixelFormat)(THIS_ LPDDPIXELFORMAT) PURE;
    STDMETHOD(GetSurfaceDesc)(THIS_ LPDDSURFACEDESC2) PURE; // 22
    STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC2) PURE;
    STDMETHOD(IsLost)(THIS) PURE; // 24
    STDMETHOD(Lock)(THIS_ LPRECT,LPDDSURFACEDESC2,DWORD,HANDLE) PURE;
    STDMETHOD(ReleaseDC)(THIS_ HDC) PURE; // 26
    STDMETHOD(Restore)(THIS) PURE;
    STDMETHOD(SetClipper)(THIS_ LPDIRECTDRAWCLIPPER) PURE; // 28
    STDMETHOD(SetColorKey)(THIS_ DWORD, LPDDCOLORKEY) PURE;
    STDMETHOD(SetOverlayPosition)(THIS_ LONG, LONG ) PURE; // 30
    STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE) PURE;
    STDMETHOD(Unlock)(THIS_ LPRECT) PURE; // 32
    STDMETHOD(UpdateOverlay)(THIS_ LPRECT, LPDIRECTDRAWSURFACE4,LPRECT,DWORD, LPDDOVERLAYFX) PURE;
    STDMETHOD(UpdateOverlayDisplay)(THIS_ DWORD) PURE; // 34
    STDMETHOD(UpdateOverlayZOrder)(THIS_ DWORD, LPDIRECTDRAWSURFACE4) PURE;
    /*** Added in the v2 interface ***/
    STDMETHOD(GetDDInterface)(THIS_ LPVOID FAR *) PURE; // 36
    STDMETHOD(PageLock)(THIS_ DWORD) PURE;
    STDMETHOD(PageUnlock)(THIS_ DWORD) PURE; // 38
    /*** Added in the v3 interface ***/
    STDMETHOD(SetSurfaceDesc)(THIS_ LPDDSURFACEDESC2, DWORD) PURE; // 39
    /*** Added in the v4 interface ***/
    STDMETHOD(SetPrivateData)(THIS_ REFGUID, LPVOID, DWORD, DWORD) PURE; // 40
    STDMETHOD(GetPrivateData)(THIS_ REFGUID, LPVOID, LPDWORD) PURE;
    STDMETHOD(FreePrivateData)(THIS_ REFGUID) PURE; // 42
    STDMETHOD(GetUniquenessValue)(THIS_ LPDWORD) PURE;
    STDMETHOD(ChangeUniquenessValue)(THIS) PURE; // 44
};


DECLARE_INTERFACE_(IDirectDrawPalette,IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE; // 0
    STDMETHOD_(ULONG,AddRef)(THIS) PURE; // 1
    STDMETHOD_(ULONG,Release)(THIS) PURE; // 2
    /*** IDirectDrawPalette methods ***/
    STDMETHOD(GetCaps)(THIS_ LPDWORD lpdwCaps) PURE; // 3
    STDMETHOD(GetEntries)(THIS_ DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries) PURE; // 4
    STDMETHOD(Initialize)(THIS_ struct IDirectDraw *ddraw, DWORD flags, PALETTEENTRY *color_table) PURE; // 5
    STDMETHOD(SetEntries)(THIS_ DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries) PURE; // 6
};

#endif

//Ddraw.lib
HACKY_IMPORT_BEGIN(DirectDrawCreate)
  hacky_printf("lpGUID 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lplpDD 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("pUnkOuter 0x%" PRIX32 "\n", stack[3]);
  *(Address*)Memory(stack[2]) = CreateInterface("IDirectDraw4", 200, 100);
  eax = 0; // DD_OK
  esp += 3 * 4;
HACKY_IMPORT_END()

// IID_IDirectDraw4


// IID_IDirectDraw4 -> STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE; // 0
HACKY_COM_BEGIN(IDirectDraw4, 0)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("riid 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("ppvObj 0x%" PRIX32 "\n", stack[3]);
  const API(IID)* iid = (const API(IID)*)Memory(stack[2]);

  char iidString[1024];
  sprintf(iidString, "%08" PRIX32 "-%04" PRIX16 "-%04" PRIX16 "-%02" PRIX8 "%02" PRIX8 "-%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8,
          iid->Data1, iid->Data2, iid->Data3,
          iid->Data4[0], iid->Data4[1], iid->Data4[2], iid->Data4[3],
          iid->Data4[4], iid->Data4[5], iid->Data4[6], iid->Data4[7]);
  info_printf("  (read iid: {%s})\n", iidString);

  char name[32];

  if (!strcmp(iidString, "9C59509A-39BD-11D1-8C4A-00C04FD930C5")) {
    strcpy(name, "IDirectDraw4");
  } else if (!strcmp(iidString, "BB223240-E72B-11D0-A9B4-00AA00C0993E")) {
    strcpy(name, "IDirect3D3");
  } else {
    assert(false);
  }

  *(Address*)Memory(stack[3]) = CreateInterface(name, 200, 100);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD_(ULONG,Release) (THIS) PURE; // 2
HACKY_COM_BEGIN(IDirectDraw4, 2)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD(CreatePalette)(THIS_ DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR * ) PURE; // 5
HACKY_COM_BEGIN(IDirectDraw4, 5)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("d 0x%" PRIX32 "\n", stack[5]);
  *(Address*)Memory(stack[4]) = CreateInterface("IDirectDrawPalette", 200, 100);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 5 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD(CreateSurface)(THIS_  LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE4 FAR *, IUnknown FAR *) PURE; // 6
HACKY_COM_BEGIN(IDirectDraw4, 6)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);

  Address surfaceAddress = CreateInterface("IDirectDrawSurface4", 50, 100);
  API(DirectDrawSurface4)* surface = (API(DirectDrawSurface4)*)Memory(surfaceAddress);

  *(Address*)Memory(stack[3]) = surfaceAddress;

  API(DDSURFACEDESC2)* desc = (API(DDSURFACEDESC2)*)Memory(stack[2]);

  info_printf("dwSize = %" PRIu32 "\n", desc->dwSize);
  info_printf("dwFlags = 0x%08" PRIX32 "\n", desc->dwFlags);
  info_printf("ddsCaps.dwCaps = 0x%08" PRIX32 "\n", desc->ddsCaps.dwCaps);
  info_printf("dwWidth = %" PRIu32 "\n", desc->dwWidth);
  info_printf("dwHeight = %" PRIu32 "\n", desc->dwHeight);

  info_printf("ddpfPixelFormat.dwSize = %" PRIu32 "\n", desc->ddpfPixelFormat.dwSize);
  info_printf("ddpfPixelFormat.dwFlags = 0x%08" PRIX32 "\n", desc->ddpfPixelFormat.dwFlags);

  info_printf("ddpfPixelFormat.dwRGBBitCount = %" PRIu32 "\n", desc->ddpfPixelFormat.dwRGBBitCount);
  info_printf("ddpfPixelFormat.dwRBitMask =        0x%08" PRIX32 "\n", desc->ddpfPixelFormat.dwRBitMask);
  info_printf("ddpfPixelFormat.dwGBitMask =        0x%08" PRIX32 "\n", desc->ddpfPixelFormat.dwGBitMask);
  info_printf("ddpfPixelFormat.dwBBitMask =        0x%08" PRIX32 "\n", desc->ddpfPixelFormat.dwBBitMask);
  info_printf("ddpfPixelFormat.dwRGBAlphaBitMask = 0x%08" PRIX32 "\n", desc->ddpfPixelFormat.dwRGBAlphaBitMask);


  memcpy(&surface->desc, desc, sizeof(API(DDSURFACEDESC2)));

enum {
  API(DDSD_PITCH) = 0x00000008l
};

  surface->desc.dwFlags = API(DDSD_PITCH);
  surface->desc.lPitch = surface->desc.dwWidth * desc->ddpfPixelFormat.dwRGBBitCount / 8;

  if (desc->ddsCaps.dwCaps & API(DDSCAPS_TEXTURE)) {
    // FIXME: Delay this until the interface is queried the first time?!
    surface->texture = CreateInterface("IDirect3DTexture2", 20, 100);
    API(Direct3DTexture2)* texture = (API(Direct3DTexture2)*)Memory(surface->texture);
    texture->surface = surfaceAddress;
    glGenTextures(1, &texture->handle);
    info_printf("GL handle is %d\n", texture->handle);
  } else {
    //FIXME: only added to catch bugs, null pointer should work
    surface->texture = CreateInterface("invalid", 200, 100);

    //FIXME: WTF is this shit?!
    API(Direct3DTexture2)* texture = (API(Direct3DTexture2)*)Memory(surface->texture);
    glGenTextures(1, &texture->handle);
    //assert(false);
  }

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 4 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD(EnumDisplayModes)( THIS_ DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2 ) PURE; // 8
HACKY_COM_BEGIN(IDirectDraw4, 8)
  hacky_printf("EnumDisplayModes\n");
  uint32_t a = stack[2];
  uint32_t b = stack[3];
  uint32_t c = stack[4];
  Address d = stack[5];
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("d 0x%" PRIX32 "\n", d);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 5 * 4;
  // Push a call to the callback onto the stack.. this is some ugly hack..

  // Convention is PASCAL

  esp -= 4;
  *(uint32_t*)Memory(esp) = returnAddress;

  {
    esp -= 4;
    *(uint32_t*)Memory(esp) = c; // user pointer

    esp -= 4;
    Address descAddress = Allocate(sizeof(API(DDSURFACEDESC2)));
    API(DDSURFACEDESC2)* desc = (API(DDSURFACEDESC2)*) Memory(descAddress);
    desc->ddpfPixelFormat.dwFlags = API(DDPF_RGB);
    desc->ddpfPixelFormat.dwRGBBitCount = 16;
    desc->dwWidth = 640;
    desc->dwHeight = 480;
    desc->lpSurface = 0x01010101;
    *(uint32_t*)Memory(esp) = descAddress; // API(DDSURFACEDESC2)*

    // Emulate the call
    esp -= 4;
    *(uint32_t*)Memory(esp) = clearEax; // Return to clear eax
    eip = d;
    info_printf("  Callback at 0x%" PRIX32 "\n", eip);
    //FIXME: Add a hook which returns 0
  }
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD(FlipToGDISurface)(THIS) PURE; // 10
HACKY_COM_BEGIN(IDirectDraw4, 10)
  hacky_printf("FlipToGDISurface\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  //GLFWwindow* glfwWindow = Application::Get()->window();

  //SDL_GL_SwapWindow(sdlWindow);
  Application::CurrentGame()->SwapBuffer();
  //glfwSwapBuffers(glfwWindow);

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD(GetCaps)( THIS_ LPDDCAPS, LPDDCAPS) PURE; // 11
HACKY_COM_BEGIN(IDirectDraw4, 11)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]); // HAL
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]); // SW

//FIXME: !!!!!!!!! Set some caps?!

// (+8) & 0x80000 ? 0 : 1; // #define DDCAPS2_CANRENDERWINDOWED 0x00080000
// (+4) & 1 ? 1 : 0; // #define DDCAPS_3D	0x00000001
// (+64)
// (+60)

#if 1
  API(DDCAPS)* halCaps = (API(DDCAPS)* ) Memory(stack[2]);
  API(DDCAPS)* swCaps = (API(DDCAPS)* ) Memory(stack[3]);

  info_printf("halCaps is %d bytes (known: %d bytes)\n", halCaps->dwSize, sizeof(API(DDCAPS)));

  halCaps->dwCaps = API(DDCAPS_3D) | API(DDCAPS_BLTDEPTHFILL);
  halCaps->dwCaps2 = API(DDCAPS2_CANRENDERWINDOWED);

  halCaps->dwVidMemTotal = 16*1024*1024; // 16MiB VRAM free :)
  halCaps->dwVidMemFree = 12*1024*1024; // 12MiB VRAM free :(

#endif

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD(Initialize)(THIS_ GUID FAR *) PURE; // 18
HACKY_COM_BEGIN(IDirectDraw4, 18)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 ->    STDMETHOD(RestoreDisplayMode)(THIS) PURE; // 19
HACKY_COM_BEGIN(IDirectDraw4, 19)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD) PURE; // 20
HACKY_COM_BEGIN(IDirectDraw4, 20)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD(SetDisplayMode)(THIS_ DWORD, DWORD,DWORD, DWORD, DWORD) PURE; // 21
HACKY_COM_BEGIN(IDirectDraw4, 21)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("d 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("e 0x%" PRIX32 "\n", stack[6]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 6 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD(GetAvailableVidMem)(THIS_ LPDDSCAPS2, LPDWORD, LPDWORD) PURE; // 23
HACKY_COM_BEGIN(IDirectDraw4, 23)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 4 * 4;
HACKY_COM_END()

// IID_IDirectDraw4 -> STDMETHOD(GetDeviceIdentifier)(THIS_ LPDDDEVICEIDENTIFIER, DWORD ) PURE; // 27
HACKY_COM_BEGIN(IDirectDraw4, 27)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()








// IDirectDrawSurface4

// IDirectDrawSurface4 -> STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE; // 0
HACKY_COM_BEGIN(IDirectDrawSurface4, 0)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  API(DirectDrawSurface4)* This = (API(DirectDrawSurface4)*)Memory(stack[1]);
  const API(IID)* iid = (const API(IID)*)Memory(stack[2]);
  info_printf("  (read iid: {%08" PRIX32 "-%04" PRIX16 "-%04" PRIX16 "-%02" PRIX8 "%02" PRIX8 "-%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "})\n",
         iid->Data1, iid->Data2, iid->Data3,
         iid->Data4[0], iid->Data4[1], iid->Data4[2], iid->Data4[3],
         iid->Data4[4], iid->Data4[5], iid->Data4[6], iid->Data4[7]);
  if (iid->Data1 == 0x93281502) { //FIXME: Check for full GUID (Direct3DTexture2)
    info_printf("Returning texture 0x%" PRIX32 "\n", This->texture);
    *(Address*)Memory(stack[3]) = This->texture;
  } else {
    assert(false);
  }
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD_(ULONG,AddRef) (THIS)  PURE; // 1
HACKY_COM_BEGIN(IDirectDrawSurface4, 1)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 1; // New reference count
  esp += 1 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD_(ULONG,Release)       (THIS) PURE; //2
HACKY_COM_BEGIN(IDirectDrawSurface4, 2)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD(AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE4) PURE; // 3
HACKY_COM_BEGIN(IDirectDrawSurface4, 3)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD(Blt)(THIS_ LPRECT,LPDIRECTDRAWSURFACE4, LPRECT,DWORD, LPDDBLTFX) PURE; // 5
HACKY_COM_BEGIN(IDirectDrawSurface4, 5)
  hacky_printf("Blt\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  uint32_t d = stack[5];
  uint32_t e = stack[6];
  hacky_printf("d 0x%08" PRIX32 "\n", d);
  hacky_printf("e 0x%" PRIX32 "\n", e);

  API(DirectDrawSurface4)* This = (API(DirectDrawSurface4)*)Memory(stack[1]);

  API(DDBLTFX)* bltfx = (API(DDBLTFX)*) Memory(e);

  assert((d & ~(API(DDBLT_COLORFILL) | API(DDBLT_WAIT) | API(DDBLT_DEPTHFILL))) == 0);

  if (d & API(DDBLT_WAIT)) {
    // nop
  }

  if (d & API(DDBLT_COLORFILL)) {
    assert(!(This->desc.ddsCaps.dwCaps & API(DDSCAPS_ZBUFFER)));

    //FIXME: Why is this zero during startup?!
    if (This->desc.ddpfPixelFormat.dwRGBBitCount != 0) {
      assert(This->desc.ddpfPixelFormat.dwRGBBitCount == 16);
    }

    glClearColor(((bltfx->dwFillColor >> 11) & 0x1F) / 31.0f,
                 ((bltfx->dwFillColor >> 5) & 0x3F) / 63.0f,
                 (bltfx->dwFillColor & 0x1F) / 31.0f,
                 255.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  if (d & API(DDBLT_DEPTHFILL)) {
    assert(This->desc.ddsCaps.dwCaps & API(DDSCAPS_ZBUFFER));
    assert(This->desc.ddpfPixelFormat.dwZBufferBitDepth == 16);

    glDepthMask(GL_TRUE);
    assert(bltfx->dwFillDepth = 0xFFFF);
    glClearDepthf(1.0f); //FIXME!!
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 6 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD(DeleteAttachedSurface)(THIS_ DWORD,LPDIRECTDRAWSURFACE4) PURE; // 8
HACKY_COM_BEGIN(IDirectDrawSurface4, 8)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD(Flip)(THIS_ LPDIRECTDRAWSURFACE4, DWORD) PURE; // 11
HACKY_COM_BEGIN(IDirectDrawSurface4, 11)
  hacky_printf("Flip\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  //GLFWwindow* glfwWindow = Application::Get()->window();
  
  //SDL_GL_SwapWindow(sdlWindow);
  //glfwSwapBuffers(glfwWindow);
  Application::CurrentGame()->SwapBuffer();

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD(GetAttachedSurface)(THIS_ LPDDSCAPS2, LPDIRECTDRAWSURFACE4 FAR *) PURE; // 12
HACKY_COM_BEGIN(IDirectDrawSurface4, 12)
  //FIXME: Don't allow this to run unlimited times, it wastes memory like crazy.
  //       Instead, do it properly..
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  API(DDSCAPS2)* caps = (API(DDSCAPS2)*)Memory(stack[2]);

  info_printf("dwCaps = 0x%08" PRIX32 "\n", caps->dwCaps);

  if (caps->dwCaps & API(DDSCAPS_MIPMAP)) {
    //FIXME: This is probably BAD!
    info_printf("Redirecting to itself\n");
    *(Address*)Memory(stack[3]) = stack[1];
  } else {
    info_printf("Creating new dummy surface\n");
    Address surfaceAddress = CreateInterface("IDirectDrawSurface4", 50, 100);
    API(DirectDrawSurface4)* surface = (API(DirectDrawSurface4)*)Memory(surfaceAddress);
    memset(&surface->desc, 0x00, sizeof(surface->desc));

    // FIXME: 32 bit would be ignored by the game. Keeps falling back to 16
    surface->desc.ddpfPixelFormat.dwRGBBitCount = 16;

    surface->texture = 0;
    *(Address*)Memory(stack[3]) = surfaceAddress;
  }
  //FIXME: Used to retrieve surface for mipmaps?!
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD(GetDC)(THIS_ HDC FAR *) PURE; // 17
HACKY_COM_BEGIN(IDirectDrawSurface4, 17)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

  API(DirectDrawSurface4)* This = (API(DirectDrawSurface4)*)Memory(stack[1]);
  if (This->texture != 0) {
    API(Direct3DTexture2)* texture = (API(Direct3DTexture2)*)Memory(This->texture);
    info_printf("Returning GL tex handle %d\n", texture->handle);
    *(Address*)Memory(stack[2]) = texture->handle;
  } else {
    info_printf("Invalid GL tex handle\n");
    *(Address*)Memory(stack[2]) = 0;
  }

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD(GetSurfaceDesc)(THIS_ LPDDSURFACEDESC2) PURE; // 22
HACKY_COM_BEGIN(IDirectDrawSurface4, 22)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

  API(DDSURFACEDESC2)* desc = (API(DDSURFACEDESC2)*)Memory(stack[2]);
  //FIXME?!

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD(Lock)(THIS_ LPRECT,LPDDSURFACEDESC2,DWORD,HANDLE) PURE; // 25
HACKY_COM_BEGIN(IDirectDrawSurface4, 25)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("d 0x%" PRIX32 "\n", stack[5]);

  API(DirectDrawSurface4)* This = (API(DirectDrawSurface4)*)Memory(stack[1]);

  assert(stack[2] == 0);
  assert(stack[5] == 0);

  //Hack: Part 1: check if we already have this surface in RAM
  if (This->desc.lpSurface == 0) {
    This->desc.lpSurface = Allocate(This->desc.dwHeight * This->desc.lPitch);
    memset(Memory(This->desc.lpSurface), 0x77, This->desc.dwHeight * This->desc.lPitch);
  }


  if (This->desc.ddsCaps.dwCaps & API(DDSCAPS_ZBUFFER)) {
    assert(This->desc.lPitch == 2 * This->desc.dwWidth);

    uint8_t* pixel_buffer = (uint8_t*)Memory(This->desc.lpSurface);
    glReadPixels(0, 0, This->desc.dwWidth, This->desc.dwHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, pixel_buffer);

    // Reserve a buffer for swapping lines during image flipping
    static size_t line_buffer_size = 0;
    static uint8_t* line_buffer = NULL;
    size_t line_size = This->desc.dwWidth * 2;
    if (line_size > line_buffer_size) {
      line_buffer = (uint8_t*)realloc(line_buffer, line_size);
      line_buffer_size = line_size;
    }

    // Do the vertical flip
    for(unsigned int i = 0; i < This->desc.dwHeight / 2; i++) {
      uint8_t* line_a = (uint8_t*)&pixel_buffer[This->desc.lPitch * i];
      uint8_t* line_b = (uint8_t*)&pixel_buffer[This->desc.lPitch * (This->desc.dwHeight - 1 - i)];
      memcpy(line_buffer, line_a, line_size);
      memcpy(line_a, line_b, line_size);
      memcpy(line_b, line_buffer, line_size);
    }

  }


  API(DDSURFACEDESC2)* desc = (API(DDSURFACEDESC2)*)Memory(stack[3]);
  memcpy(desc, &This->desc, sizeof(API(DDSURFACEDESC2)));

  info_printf("%d x %d (pitch: %d); bpp = %d; at 0x%08X\n", desc->dwWidth, desc->dwHeight, desc->lPitch, desc->ddpfPixelFormat.dwRGBBitCount, desc->lpSurface);
#if 0
  desc->dwWidth = 16;
  desc->dwHeight = 16;
  desc->lPitch = desc->dwWidth * 4; // 2 = number of bytes per pixel
#endif



  eax = 0; // FIXME: No idea what this expects to return..
  esp += 5 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD(ReleaseDC)(THIS_ HDC) PURE; // 26
HACKY_COM_BEGIN(IDirectDrawSurface4, 26)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()

// IDirectDrawSurface4 -> STDMETHOD(Unlock)(THIS_ LPRECT) PURE; // 32
HACKY_COM_BEGIN(IDirectDrawSurface4, 32)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

  assert(stack[2] == 0);

  API(DirectDrawSurface4)* This = (API(DirectDrawSurface4)*)Memory(stack[1]);

  API(DDSURFACEDESC2)* desc = &This->desc;

  API(Direct3DTexture2)* texture = (API(Direct3DTexture2)*)Memory(This->texture);

  GLint previousTexture = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
  glBindTexture(GL_TEXTURE_2D, texture->handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  if (desc->ddpfPixelFormat.dwRGBBitCount == 32) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, desc->dwWidth, desc->dwHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, Memory(desc->lpSurface));
  } else {
    if (desc->ddpfPixelFormat.dwRGBAlphaBitMask == 0x8000) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, desc->dwWidth, desc->dwHeight, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, Memory(desc->lpSurface));
    } else {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, desc->dwWidth, desc->dwHeight, 0, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV, Memory(desc->lpSurface));
    }
  }
  glBindTexture(GL_TEXTURE_2D, previousTexture);

//Hack: part 2: don't free this to keep data in RAM. see lock for part 1
#if 0
  Free(desc->lpSurface);
  desc->lpSurface = 0;
#endif

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()



// IDirectDrawPalette -> STDMETHOD_(ULONG,Release)(THIS) PURE; // 2
HACKY_COM_BEGIN(IDirectDrawPalette, 2)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()





// DirectDraw

HACKY_IMPORT_BEGIN(DirectDrawEnumerateA)
  Address lpCallback = stack[1];
  Address lpContext = stack[2];
  hacky_printf("lpCallback 0x%" PRIX32 "\n", lpCallback); // LPDDENUMCALLBACKA
  hacky_printf("lpContext 0x%" PRIX32 "\n", lpContext);
  eax = 0; // HRESULT -> non-negative means success
  esp += 2 * 4;
#if 1
  // Push a call to the callback onto the stack.. this is some ugly hack..

  // Convention is PASCAL

  esp -= 4;
  *(uint32_t*)Memory(esp) = returnAddress;

  {
    esp -= 4;
    *(uint32_t*)Memory(esp) = lpContext; // lpContext

    esp -= 4;
    *(uint32_t*)Memory(esp) = Allocate(200); // lpDriverName

    // Used to identifty drawing device
    esp -= 4;
    Address lpDriverDescription = Allocate(200);
    sprintf((char*)Memory(lpDriverDescription), "%s", "OpenSWE1R D3D");
    *(uint32_t*)Memory(esp) = lpDriverDescription;

    // Used as parameter in Direct Draw `Initialize`
    esp -= 4;
    *(uint32_t*)Memory(esp) = Allocate(200); // lpGUID

    // Emulate the call
    esp -= 4;
    *(uint32_t*)Memory(esp) = clearEax;
    eip = lpCallback;

    info_printf("  Callback at 0x%" PRIX32 "\n", eip);
    //FIXME: Add a hook which returns 0
  }
#endif
HACKY_IMPORT_END()

