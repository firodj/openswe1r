// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "../emulation.h"

typedef struct {
  uint32_t dwSize;
  uint32_t dwFlags;
  uint32_t dwFourCC;
  union {
    uint32_t dwRGBBitCount;
    uint32_t dwYUVBitCount;
    uint32_t dwZBufferBitDepth;
    uint32_t dwAlphaBitDepth;
    uint32_t dwLuminanceBitCount;
    uint32_t dwBumpBitCount;
    uint32_t dwPrivateFormatBitCount;
  } dummy1;
  union {
    uint32_t dwRBitMask;
    uint32_t dwYBitMask;
    uint32_t dwStencilBitDepth;
    uint32_t dwLuminanceBitMask;
    uint32_t dwBumpDuBitMask;
    uint32_t dwOperations;
  } dummy2;
  union {
    uint32_t  dwGBitMask;
    uint32_t  dwUBitMask;
    uint32_t  dwZBitMask;
    uint32_t  dwBumpDvBitMask;
    struct {
      uint16_t wFlipMSTypes;
      uint16_t wBltMSTypes;
    } MultiSampleCaps;
  } dummy3;
  union {
    uint32_t dwBBitMask;
    uint32_t dwVBitMask;
    uint32_t dwStencilBitMask;
    uint32_t dwBumpLuminanceBitMask;
  } dummy4;
  union {
    uint32_t dwRGBAlphaBitMask;
    uint32_t dwYUVAlphaBitMask;
    uint32_t dwLuminanceAlphaBitMask;
    uint32_t dwRGBZBitMask;
    uint32_t dwYUVZBitMask;
  } dummy5;
} API(DDPIXELFORMAT);

#if 0


DECLARE_INTERFACE_(IDirect3D3, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE; // 0
    STDMETHOD_(ULONG,AddRef)(THIS) PURE; // 1
    STDMETHOD_(ULONG,Release)(THIS) PURE; // 2

    /*** IDirect3D3 methods ***/
    STDMETHOD(EnumDevices)(THIS_ LPD3DENUMDEVICESCALLBACK,LPVOID) PURE; // 3
    STDMETHOD(CreateLight)(THIS_ LPDIRECT3DLIGHT*,LPUNKNOWN) PURE; // 4
    STDMETHOD(CreateMaterial)(THIS_ LPDIRECT3DMATERIAL3*,LPUNKNOWN) PURE; // 5
    STDMETHOD(CreateViewport)(THIS_ LPDIRECT3DVIEWPORT3*,LPUNKNOWN) PURE; // 6
    STDMETHOD(FindDevice)(THIS_ LPD3DFINDDEVICESEARCH,LPD3DFINDDEVICERESULT) PURE;
    STDMETHOD(CreateDevice)(THIS_ REFCLSID,LPDIRECTDRAWSURFACE4,LPDIRECT3DDEVICE3*,LPUNKNOWN) PURE; // 8
    STDMETHOD(CreateVertexBuffer)(THIS_ LPD3DVERTEXBUFFERDESC,LPDIRECT3DVERTEXBUFFER*,uint32_t,LPUNKNOWN) PURE;
    STDMETHOD(EnumZBufferFormats)(THIS_ REFCLSID,LPD3DENUMPIXELFORMATSCALLBACK,LPVOID) PURE; // 10
    STDMETHOD(EvictManagedTextures)(THIS) PURE;
};


#endif


#if 0

DECLARE_INTERFACE_(IDirect3DDevice3, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE; // 0
    STDMETHOD_(ULONG,AddRef)(THIS) PURE; // 1
    STDMETHOD_(ULONG,Release)(THIS) PURE; // 2

    /*** IDirect3DDevice3 methods ***/
    STDMETHOD(GetCaps)(THIS_ LPD3DDEVICEDESC,LPD3DDEVICEDESC) PURE; // 3
    STDMETHOD(GetStats)(THIS_ LPD3DSTATS) PURE; // 4
    STDMETHOD(AddViewport)(THIS_ LPDIRECT3DVIEWPORT3) PURE;
    STDMETHOD(DeleteViewport)(THIS_ LPDIRECT3DVIEWPORT3) PURE; // 6
    STDMETHOD(NextViewport)(THIS_ LPDIRECT3DVIEWPORT3,LPDIRECT3DVIEWPORT3*,DWORD) PURE;
    STDMETHOD(EnumTextureFormats)(THIS_ LPD3DENUMPIXELFORMATSCALLBACK,LPVOID) PURE; // 8
    STDMETHOD(BeginScene)(THIS) PURE;
    STDMETHOD(EndScene)(THIS) PURE; // 10
    STDMETHOD(GetDirect3D)(THIS_ LPDIRECT3D3*) PURE;
    STDMETHOD(SetCurrentViewport)(THIS_ LPDIRECT3DVIEWPORT3) PURE; // 12
    STDMETHOD(GetCurrentViewport)(THIS_ LPDIRECT3DVIEWPORT3 *) PURE;
    STDMETHOD(SetRenderTarget)(THIS_ LPDIRECTDRAWSURFACE4,DWORD) PURE; // 14
    STDMETHOD(GetRenderTarget)(THIS_ LPDIRECTDRAWSURFACE4 *) PURE;
    STDMETHOD(Begin)(THIS_ D3DPRIMITIVETYPE,DWORD,DWORD) PURE; // 16
    STDMETHOD(BeginIndexed)(THIS_ D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,DWORD) PURE;
    STDMETHOD(Vertex)(THIS_ LPVOID) PURE; // 18
    STDMETHOD(Index)(THIS_ WORD) PURE;
    STDMETHOD(End)(THIS_ DWORD) PURE; // 20
    STDMETHOD(GetRenderState)(THIS_ D3DRENDERSTATETYPE,LPDWORD) PURE;
    STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE,DWORD) PURE; // 22
    STDMETHOD(GetLightState)(THIS_ D3DLIGHTSTATETYPE,LPDWORD) PURE;
    STDMETHOD(SetLightState)(THIS_ D3DLIGHTSTATETYPE,DWORD) PURE; // 24
    STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE,LPD3DMATRIX) PURE; // 25
    STDMETHOD(GetTransform)(THIS_ D3DTRANSFORMSTATETYPE,LPD3DMATRIX) PURE; // 26
    STDMETHOD(MultiplyTransform)(THIS_ D3DTRANSFORMSTATETYPE,LPD3DMATRIX) PURE;
    STDMETHOD(DrawPrimitive)(THIS_ D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,DWORD) PURE; // 28
    STDMETHOD(DrawIndexedPrimitive)(THIS_ D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,LPWORD,DWORD,DWORD) PURE;
    STDMETHOD(SetClipStatus)(THIS_ LPD3DCLIPSTATUS) PURE; // 30
    STDMETHOD(GetClipStatus)(THIS_ LPD3DCLIPSTATUS) PURE;
    STDMETHOD(DrawPrimitiveStrided)(THIS_ D3DPRIMITIVETYPE,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,DWORD) PURE; // 32
    STDMETHOD(DrawIndexedPrimitiveStrided)(THIS_ D3DPRIMITIVETYPE,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,LPWORD,DWORD,DWORD) PURE;
    STDMETHOD(DrawPrimitiveVB)(THIS_ D3DPRIMITIVETYPE,LPDIRECT3DVERTEXBUFFER,DWORD,DWORD,DWORD) PURE; // 34
    STDMETHOD(DrawIndexedPrimitiveVB)(THIS_ D3DPRIMITIVETYPE,LPDIRECT3DVERTEXBUFFER,LPWORD,DWORD,DWORD) PURE;
    STDMETHOD(ComputeSphereVisibility)(THIS_ LPD3DVECTOR,LPD3DVALUE,DWORD,DWORD,LPDWORD) PURE; // 36
    STDMETHOD(GetTexture)(THIS_ DWORD,LPDIRECT3DTEXTURE2 *) PURE;
    STDMETHOD(SetTexture)(THIS_ DWORD,LPDIRECT3DTEXTURE2) PURE; // 38
    STDMETHOD(GetTextureStageState)(THIS_ DWORD,D3DTEXTURESTAGESTATETYPE,LPDWORD) PURE;
    STDMETHOD(SetTextureStageState)(THIS_ DWORD,D3DTEXTURESTAGESTATETYPE,DWORD) PURE; // 40
    STDMETHOD(ValidateDevice)(THIS_ LPDWORD) PURE;
};

#endif

#if 0


DECLARE_INTERFACE_(IDirect3DViewport3, IDirect3DViewport2)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE; // 0
    STDMETHOD_(ULONG,AddRef)(THIS) PURE; // 1
    STDMETHOD_(ULONG,Release)(THIS) PURE; // 2

    /*** IDirect3DViewport2 methods ***/
    STDMETHOD(Initialize)(THIS_ LPDIRECT3D) PURE; // 3
    STDMETHOD(GetViewport)(THIS_ LPD3DVIEWPORT) PURE; // 4
    STDMETHOD(SetViewport)(THIS_ LPD3DVIEWPORT) PURE;
    STDMETHOD(TransformVertices)(THIS_ DWORD,LPD3DTRANSFORMDATA,DWORD,LPDWORD) PURE; // 6
    STDMETHOD(LightElements)(THIS_ DWORD,LPD3DLIGHTDATA) PURE;
    STDMETHOD(SetBackground)(THIS_ D3DMATERIALHANDLE) PURE; // 8
    STDMETHOD(GetBackground)(THIS_ LPD3DMATERIALHANDLE,LPBOOL) PURE;
    STDMETHOD(SetBackgroundDepth)(THIS_ LPDIRECTDRAWSURFACE) PURE; // 10
    STDMETHOD(GetBackgroundDepth)(THIS_ LPDIRECTDRAWSURFACE*,LPBOOL) PURE;
    STDMETHOD(Clear)(THIS_ DWORD,LPD3DRECT,DWORD) PURE; // 12
    STDMETHOD(AddLight)(THIS_ LPDIRECT3DLIGHT) PURE;
    STDMETHOD(DeleteLight)(THIS_ LPDIRECT3DLIGHT) PURE; // 14
    STDMETHOD(NextLight)(THIS_ LPDIRECT3DLIGHT,LPDIRECT3DLIGHT*,DWORD) PURE;
    STDMETHOD(GetViewport2)(THIS_ LPD3DVIEWPORT2) PURE; // 16
    STDMETHOD(SetViewport2)(THIS_ LPD3DVIEWPORT2) PURE;
    STDMETHOD(SetBackgroundDepth2)(THIS_ LPDIRECTDRAWSURFACE4) PURE; // 18
    STDMETHOD(GetBackgroundDepth2)(THIS_ LPDIRECTDRAWSURFACE4*,LPBOOL) PURE;
    STDMETHOD(Clear2)(THIS_ DWORD,LPD3DRECT,DWORD,D3DCOLOR,D3DVALUE,DWORD) PURE; // 20
};

#endif



#if 0


DECLARE_INTERFACE_(IDirect3DTexture2, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE; // 0
    STDMETHOD_(ULONG,AddRef)(THIS) PURE; // 1
    STDMETHOD_(ULONG,Release)(THIS) PURE; // 2

    /*** IDirect3DTexture2 methods ***/
    STDMETHOD(GetHandle)(THIS_ LPDIRECT3DDEVICE2,LPD3DTEXTUREHANDLE) PURE; // 3
    STDMETHOD(PaletteChanged)(THIS_ DWORD,DWORD) PURE; // 4
    STDMETHOD(Load)(THIS_ LPDIRECT3DTEXTURE2) PURE; // 5
};


#endif
