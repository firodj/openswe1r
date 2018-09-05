// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "../main.h"
#include "../windows.h"
#include "../export.h"
#include "../emulation.h"
#include "glad/glad.h"

#include <string.h>
#include <assert.h>
#include "ddraw.h"
#include "d3d.h"

#include "../Application.hpp"
#include "../Game.hpp"

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

int tex0Blend;
bool depthMask;
GLenum destBlend;
GLenum srcBlend;
bool alphaTest;
uint32_t fogColor; // ARGB
bool fogEnable;
int fogMode;
float fogStart;
float fogEnd;
float projectionMatrix[16];
float clipScale[3];
float clipOffset[3];


static void PrintVertices(unsigned int vertexFormat, Address address, unsigned int count) {
    unsigned int texCount = ((vertexFormat & 0xF00) >> 8);
    GLsizei stride = 4 * 4 + 4 + 4 + texCount * 8;
    
    uint32_t* p = (uint32_t*)Memory(address);
    for(unsigned int i = 0; i < count; i++) {
        float* f = (float*)p;
        info_printf("  %d.    %f %f %f %f 0x%08" PRIX32 " 0x%08" PRIX32, i, f[0], f[1], f[2], f[3], p[4], p[5]);
        p += 6;
        if (texCount >= 1) {
            info_printf(" %f %f", f[6], f[7]);
            p += 2;
        }
        if (texCount >= 2) {
            info_printf(" %f %f", f[8], f[9]);
            p += 2;
        }
        info_printf("\n");
    }
}

static void LoadIndices(Address address, unsigned int count) {
    static GLuint buffer = 0;
    if (buffer == 0) {
        glGenBuffers(1, &buffer);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * 2, Memory(address), GL_STREAM_DRAW);
}

static void LoadVertices(unsigned int vertexFormat, Address address, unsigned int count) {
    unsigned int texCount = ((vertexFormat & 0xF00) >> 8);
    GLsizei stride = 4 * 4 + 4 + 4 + texCount * 8;
    
    static GLuint buffer = 0;
    if (buffer == 0) {
        glGenBuffers(1, &buffer);
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, count * stride, Memory(address), GL_STREAM_DRAW);
}



static GLenum SetupRenderer(unsigned int primitiveType, unsigned int vertexFormat) {
    unsigned int texCount = ((vertexFormat & 0xF00) >> 8);
    GLsizei stride = 4 * 4 + 4 + 4 + texCount * 8;
    // Re-Volt only uses D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR OR'd with either D3DFVF_TEX{0,1,2}
    
    assert(texCount == 1);
    
    GLuint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&program);
    
    GLint positionIn = glGetAttribLocation(program, "positionIn");
    if (positionIn != -1) {
        glEnableVertexAttribArray(positionIn);
        glVertexAttribPointer(positionIn, 4, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)0);
    }
    
    GLint diffuseIn = glGetAttribLocation(program, "diffuseIn");
    if (diffuseIn != -1) {
        glEnableVertexAttribArray(diffuseIn);
        glVertexAttribPointer(diffuseIn, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (const GLvoid*)16);
    }
    
    GLint specularIn = glGetAttribLocation(program, "specularIn");
    if (specularIn != -1) {
        glEnableVertexAttribArray(specularIn);
        glVertexAttribPointer(specularIn, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (const GLvoid*)20);
    }
    
    GLint uv0In = glGetAttribLocation(program, "uv0In");
    if (uv0In != -1) {
        glEnableVertexAttribArray(uv0In);
        glVertexAttribPointer(uv0In, 2, GL_FLOAT, GL_TRUE, stride, (const GLvoid*)24);
    }
    
    glUniform1i(glGetUniformLocation(program, "tex0Blend"), tex0Blend);
    glUniform1i(glGetUniformLocation(program, "tex0"), 0);
    glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_FALSE, projectionMatrix);
    
    glUniform1i(glGetUniformLocation(program, "alphaTest"), alphaTest);
    
    glUniform1i(glGetUniformLocation(program, "fogMode"), fogEnable ? fogMode : API(D3DFOG_NONE));
    glUniform1f(glGetUniformLocation(program, "fogStart"), fogStart);
    glUniform1f(glGetUniformLocation(program, "fogEnd"), fogEnd);
    glUniform3f(glGetUniformLocation(program, "fogColor"),
                ((fogColor >> 16) & 0xFF) / 255.0,
                ((fogColor >> 8) & 0xFF) / 255.0,
                (fogColor & 0xFF) / 255.0);
    
    glUniform3fv(glGetUniformLocation(program, "clipScale"), 1, clipScale);
    glUniform3fv(glGetUniformLocation(program, "clipOffset"), 1, clipOffset);
    
    glBlendFunc(srcBlend, destBlend);
    
#if 0
    // Wireframe mode
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
#endif
    
    glDepthMask(depthMask ? GL_TRUE : GL_FALSE);
    
    GLenum mode;
    switch(primitiveType) {
        case 1: mode = GL_POINTS;         break; // D3DPT_POINTLIST
        case 2: mode = GL_LINES;          break; // D3DPT_LINELIST
        case 3: mode = GL_LINE_STRIP;     break; // D3DPT_LINESTRIP
        case 4: mode = GL_TRIANGLES;      break; // D3DPT_TRIANGLELIST
        case 5: mode = GL_TRIANGLE_STRIP; break; // D3DPT_TRIANGLESTRIP
        case 6: mode = GL_TRIANGLE_FAN;   break; // D3DPT_TRIANGLEFAN
        default:
            assert(false);
            break;
    }
    return mode;
}








// IDirect3D3 -> STDMETHOD_(ULONG,Release)(THIS) PURE; // 2
HACKY_COM_BEGIN(IDirect3D3, 2)
hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
eax = 0; // FIXME: No idea what this expects to return..
esp += 1 * 4;
HACKY_COM_END()

// IDirect3D3 -> STDMETHOD(EnumDevices)(THIS_ LPD3DENUMDEVICESCALLBACK,LPVOID) PURE; // 3
HACKY_COM_BEGIN(IDirect3D3, 3)
hacky_printf("EnumDevices\n");
uint32_t a = stack[2];
uint32_t b = stack[3];
hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
hacky_printf("a 0x%" PRIX32 "\n", a);
hacky_printf("b 0x%" PRIX32 "\n", b);
eax = 0; // FIXME: No idea what this expects to return..
esp += 3 * 4;
// Push a call to the callback onto the stack.. this is some ugly hack..

// Convention not specified -> stdcall?!

esp -= 4;
*(uint32_t*)Memory(esp) = returnAddress;

{
    esp -= 4;
    *(uint32_t*)Memory(esp) = b; // lpContext
    
    Address desc_addr = Allocate(sizeof(API(D3DDEVICEDESC)));
    API(D3DDEVICEDESC)* desc = (API(D3DDEVICEDESC)*)Memory(desc_addr);
    memset(desc, 0x00, sizeof(API(D3DDEVICEDESC)));
    desc->dwSize = sizeof(API(D3DDEVICEDESC));
    desc->dwFlags = 0xFFFFFFFF;
    
    desc->dwDeviceZBufferBitDepth = 16;
    
    enum {
        API(D3DPTEXTURECAPS_PERSPECTIVE) =   0x00000001L,
        API(D3DPTEXTURECAPS_ALPHA) =         0x00000004L,
        API(D3DPTEXTURECAPS_TRANSPARENCY) =  0x00000008L
    };
    
    
    desc->dpcTriCaps.dwTextureCaps = 0;
    desc->dpcTriCaps.dwTextureCaps |= API(D3DPTEXTURECAPS_PERSPECTIVE);
    desc->dpcTriCaps.dwTextureCaps |= API(D3DPTEXTURECAPS_ALPHA);
    desc->dpcTriCaps.dwTextureCaps |= API(D3DPTEXTURECAPS_TRANSPARENCY);
    
    enum {
        API(D3DPSHADECAPS_ALPHAGOURAUDBLEND) = 0x00004000L
    };
    
    desc->dpcTriCaps.dwShadeCaps = 0;
    desc->dpcTriCaps.dwShadeCaps |= API(D3DPSHADECAPS_ALPHAGOURAUDBLEND);
    
    enum {
        API(D3DPTBLENDCAPS_MODULATEALPHA) = 0x00000008L
    };
    
    desc->dpcTriCaps.dwTextureBlendCaps = 0;
    desc->dpcTriCaps.dwTextureBlendCaps |= API(D3DPTBLENDCAPS_MODULATEALPHA);
    
    desc->dwMaxVertexCount = 0xFFFF;
    
    desc->dwMinTextureWidth = 1;
    desc->dwMinTextureHeight = 1;
    desc->dwMaxTextureWidth = 2048;
    desc->dwMaxTextureHeight = 2048;
    
    esp -= 4;
    *(uint32_t*)Memory(esp) = desc_addr; // LPD3DDEVICEDESC
    
    esp -= 4;
    *(uint32_t*)Memory(esp) = desc_addr; // LPD3DDEVICEDESC
    
    esp -= 4;
    Address lpDeviceName = Allocate(200);
    sprintf((char*)Memory(lpDeviceName), "%s", "OpenSWE1R D3D");
    *(uint32_t*)Memory(esp) = lpDeviceName;
    
    // Used to identifty drawing device
    esp -= 4;
    Address lpDeviceDescription = Allocate(200);
    sprintf((char*)Memory(lpDeviceDescription), "%s", "OpenSWE1R D3D");
    *(uint32_t*)Memory(esp) = lpDeviceDescription;
    
    // Used as parameter in Direct Draw `Initialize`
    esp -= 4;
    Address guid_addr = Allocate(sizeof(API(IID)));
    API(IID)* guid = (API(IID)*)Memory(guid_addr);
    API(IID)* iid = guid;
    
    // IDirect3DHALDevice
    iid->Data1 = 0x84E63DE0;
    iid->Data2 = 0x46AA;
    iid->Data3 = 0x11CF;
    iid->Data4[0] = 0x81;
    iid->Data4[1] = 0x6F;
    iid->Data4[2] = 0x00;
    iid->Data4[3] = 0x00;
    iid->Data4[4] = 0xC0;
    iid->Data4[5] = 0x20;
    iid->Data4[6] = 0x15;
    iid->Data4[7] = 0x6E;
    info_printf("\n\n\n\n\n\n\n  (planned iid: {%08" PRIX32 "-%04" PRIX16 "-%04" PRIX16 "-%02" PRIX8 "%02" PRIX8 "-%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "})\n\n\n\n\n\n",
                iid->Data1, iid->Data2, iid->Data3,
                iid->Data4[0], iid->Data4[1], iid->Data4[2], iid->Data4[3],
                iid->Data4[4], iid->Data4[5], iid->Data4[6], iid->Data4[7]);
    *(uint32_t*)Memory(esp) = guid_addr; // lpGUID
    
    // Emulate the call
    esp -= 4;
    *(uint32_t*)Memory(esp) = clearEax; // Return to clear eax
    eip = a;
    info_printf("  Callback at 0x%" PRIX32 "\n", eip);
    //FIXME: Add a hook which returns 0
}
HACKY_COM_END()


// IDirect3D3 -> STDMETHOD(CreateViewport)(THIS_ LPDIRECT3DVIEWPORT3*,LPUNKNOWN) PURE; // 6
HACKY_COM_BEGIN(IDirect3D3, 6)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  *(Address*)Memory(stack[2]) = CreateInterface("IDirect3DViewport3", 200, 100);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirect3D3 -> STDMETHOD(CreateDevice)(THIS_ REFCLSID,LPDIRECTDRAWSURFACE4,LPDIRECT3DDEVICE3*,LPUNKNOWN) PURE; // 8
HACKY_COM_BEGIN(IDirect3D3, 8)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("d 0x%" PRIX32 "\n", stack[5]);
  *(Address*)Memory(stack[4]) = CreateInterface("IDirect3DDevice3", 200, 100);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 5 * 4;
HACKY_COM_END()

// IDirect3D3 -> STDMETHOD(EnumZBufferFormats)(THIS_ REFCLSID,LPD3DENUMPIXELFORMATSCALLBACK,LPVOID) PURE; // 10
HACKY_COM_BEGIN(IDirect3D3, 10)
  hacky_printf("EnumZBufferFormats\n");
  uint32_t b = stack[3];
  uint32_t c = stack[4];
  info_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", b);
  hacky_printf("c 0x%" PRIX32 "\n", c);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 4 * 4;
  // Push a call to the callback onto the stack.. this is some ugly hack..

  // Convention not specified -> stdcall?!

  esp -= 4;
  *(uint32_t*)Memory(esp) = returnAddress;

  {
    Address formatAddress = Allocate(sizeof(API(DDPIXELFORMAT)));
    API(DDPIXELFORMAT)* format = (API(DDPIXELFORMAT)*)Memory(formatAddress);
    format->dwSize = sizeof(API(DDPIXELFORMAT));
    format->dwFlags = 0x400; // DDPF_ZBUFFER;
    format->dwZBufferBitDepth = 16;
    
    esp -= 4;
    *(uint32_t*)Memory(esp) = c; // user pointer
    esp -= 4;
    *(uint32_t*)Memory(esp) = formatAddress; // API(DDPIXELFORMAT)*
    
    // Emulate the call
    esp -= 4;
    *(uint32_t*)Memory(esp) = clearEax; // Return to clear eax
    eip = b;
    info_printf("  Callback at 0x%" PRIX32 "\n", eip);
    //FIXME: Add a hook which returns 0
  }
HACKY_COM_END()


// IDirect3DDevice3 -> STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE; // 0
HACKY_COM_BEGIN(IDirect3DDevice3, 0)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  API(DirectDrawSurface4)* This = (API(DirectDrawSurface4)*)Memory(stack[1]);
  const API(IID)* iid = (const API(IID)*)Memory(stack[2]);
  info_printf("  (read iid: {%08" PRIX32 "-%04" PRIX16 "-%04" PRIX16 "-%02" PRIX8 "%02" PRIX8 "-%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "})\n",
            iid->Data1, iid->Data2, iid->Data3,
            iid->Data4[0], iid->Data4[1], iid->Data4[2], iid->Data4[3],
            iid->Data4[4], iid->Data4[5], iid->Data4[6], iid->Data4[7]);
#if 0
  if (iid->Data1 == 0x93281502) { //FIXME: Check for full GUID (Direct3DTexture2)
    info_printf("Returning texture 0x%" PRIX32 "\n", This->texture);
    *(Address*)Memory(stack[3]) = This->texture;
  } else {
    assert(false);
  }
#endif
  assert(false);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD_(ULONG,Release)       (THIS) PURE; //2
HACKY_COM_BEGIN(IDirect3DDevice3, 2)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD(GetCaps)(THIS_ LPD3DDEVICEDESC,LPD3DDEVICEDESC) PURE; // 3
HACKY_COM_BEGIN(IDirect3DDevice3, 3)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);

  API(D3DDEVICEDESC)* desc = (API(D3DDEVICEDESC)*)Memory(stack[2]);
  uint32_t size = desc->dwSize;
  memset(desc, 0x00, size);
  desc->dwSize = size;
  desc->dpcTriCaps.dwTextureBlendCaps = 0;
  desc->dpcTriCaps.dwTextureBlendCaps |= API(D3DPTBLENDCAPS_MODULATEALPHA);

  desc->dpcTriCaps.dwTextureBlendCaps = 0;
  desc->dpcTriCaps.dwRasterCaps |= API(D3DPRASTERCAPS_FOGTABLE);

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD(AddViewport)(THIS_ LPDIRECT3DVIEWPORT3) PURE; // 5
HACKY_COM_BEGIN(IDirect3DDevice3, 5)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD(EnumTextureFormats)(THIS_ LPD3DENUMPIXELFORMATSCALLBACK,LPVOID) PURE; // 8
HACKY_COM_BEGIN(IDirect3DDevice3, 8)
  hacky_printf("EnumTextureFormats\n");
  uint32_t a = stack[2];
  uint32_t b = stack[3];
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", a);
  hacky_printf("b 0x%" PRIX32 "\n", b);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
  // Push a call to the callback onto the stack.. this is some ugly hack..

  // Convention not specified -> stdcall?!

  esp -= 4;
  *(uint32_t*)Memory(esp) = returnAddress;

  {
    {
        Address formatAddress = Allocate(sizeof(API(DDPIXELFORMAT)));
        API(DDPIXELFORMAT)* format = (API(DDPIXELFORMAT)*)Memory(formatAddress);
        memset(format, 0x00, sizeof(API(DDPIXELFORMAT)));
        format->dwSize = sizeof(API(DDPIXELFORMAT));
        format->dwFlags = API(DDPF_RGB) | API(DDPF_ALPHAPIXELS);
        format->dwRGBBitCount = 16;
        format->dwRBitMask = 0x0F00;
        format->dwGBitMask = 0x00F0;
        format->dwBBitMask = 0x000F;
        format->dwRGBAlphaBitMask = 0xF000;
        
        esp -= 4;
        *(uint32_t*)Memory(esp) = b; // user pointer
        esp -= 4;
        *(uint32_t*)Memory(esp) = formatAddress; // API(DDPIXELFORMAT)*
        
        // Emulate a call by setting return address to where we want to go.
        esp -= 4;
        *(uint32_t*)Memory(esp) = clearEax; // Return to clear eax
    }
    {
        Address formatAddress = Allocate(sizeof(API(DDPIXELFORMAT)));
        API(DDPIXELFORMAT)* format = (API(DDPIXELFORMAT)*)Memory(formatAddress);
        memset(format, 0x00, sizeof(API(DDPIXELFORMAT)));
        format->dwSize = sizeof(API(DDPIXELFORMAT));
        format->dwFlags = API(DDPF_RGB) | API(DDPF_ALPHAPIXELS);
        format->dwRGBBitCount = 16;
        format->dwRBitMask = 0x7C00;
        format->dwGBitMask = 0x03E0;
        format->dwBBitMask = 0x001F;
        format->dwRGBAlphaBitMask = 0x8000;
        
        esp -= 4;
        *(uint32_t*)Memory(esp) = b; // user pointer
        esp -= 4;
        *(uint32_t*)Memory(esp) = formatAddress; // API(DDPIXELFORMAT)*
        
        // Emulate a call by setting return address to the callback.
        esp -= 4;
        *(uint32_t*)Memory(esp) = a; // Continue with next format
    }
#if 1
    {
        Address formatAddress = Allocate(sizeof(API(DDPIXELFORMAT)));
        API(DDPIXELFORMAT)* format = (API(DDPIXELFORMAT)*)Memory(formatAddress);
        memset(format, 0x00, sizeof(API(DDPIXELFORMAT)));
        format->dwSize = sizeof(API(DDPIXELFORMAT));
        format->dwFlags = API(DDPF_RGB) | API(DDPF_ALPHAPIXELS);
        format->dwRGBBitCount = 32;
        format->dwRBitMask = 0x00FF0000;
        format->dwGBitMask = 0x0000FF00;
        format->dwBBitMask = 0x000000FF;
        format->dwRGBAlphaBitMask = 0xFF000000;
        
        esp -= 4;
        *(uint32_t*)Memory(esp) = b; // user pointer
        esp -= 4;
        *(uint32_t*)Memory(esp) = formatAddress; // API(DDPIXELFORMAT)*
        
        // Emulate the call. We are calling the callback.
        // We also set the return address to the callback.
        esp -= 4;
        *(uint32_t*)Memory(esp) = a; // Continue with next format
        eip = a;
    }
#endif
    
    
    info_printf("  Callback at 0x%" PRIX32 "\n", eip);
    //FIXME: Add a hook which returns 0
  }
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD(BeginScene)(THIS) PURE; // 9
HACKY_COM_BEGIN(IDirect3DDevice3, 9)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD(EndScene)(THIS) PURE; // 10
HACKY_COM_BEGIN(IDirect3DDevice3, 10)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD(SetCurrentViewport)(THIS_ LPDIRECT3DVIEWPORT3) PURE; // 12
HACKY_COM_BEGIN(IDirect3DDevice3, 12)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()

static void glSet(GLenum state, bool set) {
    if (set) {
        glEnable(state);
    } else {
        glDisable(state);
    }
}

GLenum mapBlend(API(D3DBLEND) blend) {
    switch(blend) {
        case API(D3DBLEND_SRCALPHA):
            return GL_SRC_ALPHA;
        case API(D3DBLEND_INVSRCALPHA):
            return GL_ONE_MINUS_SRC_ALPHA;
        default:
            assert(false);
            return GL_ZERO;
    }
}

// IDirect3DDevice3 -> STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE,DWORD) PURE; // 22
HACKY_COM_BEGIN(IDirect3DDevice3, 22)
  silent = false;
  if (!silent) {
    hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
    hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
    hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  }
  uint32_t a = stack[2];
  uint32_t b = stack[3];
  switch(a) {
    case API(D3DRENDERSTATE_TEXTUREHANDLE):
        assert(b == 0); // Texture handle for legacy interfaces (Texture,Texture2)
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_ANTIALIAS):
        assert(b == API(D3DANTIALIAS_NONE)); // D3DANTIALIASMODE
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_TEXTUREPERSPECTIVE):
        assert(b == 1); // TRUE for perspective correction
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_ZENABLE):
        assert(b == 1); // D3DZBUFFERTYPE (or TRUE/FALSE for legacy)
        glSet(GL_DEPTH_TEST, b);
        break;
        
    case API(D3DRENDERSTATE_FILLMODE):
        assert(b == API(D3DFILL_SOLID)); // D3DFILLMODE
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_SHADEMODE):
        assert(b == API(D3DSHADE_GOURAUD)); // D3DSHADEMODE
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_MONOENABLE):
        assert(b == 0); // TRUE to enable mono rasterization
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_ZWRITEENABLE):
        assert((b == 0) || (b == 1)); // TRUE to enable z writes
        depthMask = b;
        break;
        
    case API(D3DRENDERSTATE_ALPHATESTENABLE):
        assert(b == 1); // TRUE to enable alpha tests
        alphaTest = b;
        break;
        
    case API(D3DRENDERSTATE_TEXTUREMIN):
        assert(b == API(D3DFILTER_MIPNEAREST)); // D3DTEXTUREFILTER
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_SRCBLEND):
        assert(b == API(D3DBLEND_SRCALPHA)); // D3DBLEND
        srcBlend = mapBlend((API(D3DBLEND))b);
        break;
        
    case API(D3DRENDERSTATE_DESTBLEND):
        assert(b == API(D3DBLEND_INVSRCALPHA)); // D3DBLEND
        destBlend = mapBlend((API(D3DBLEND))b);
        break;
        
    case API(D3DRENDERSTATE_TEXTUREMAPBLEND):
        assert((b == API(D3DTBLEND_MODULATE)) || (b == API(D3DTBLEND_MODULATEALPHA))); // D3DTEXTUREBLEND
        tex0Blend = b;
        break;
        
    case API(D3DRENDERSTATE_CULLMODE):
        assert(b == API(D3DCULL_NONE)); // D3DCULL
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_ZFUNC):
        assert(b == API(D3DCMP_LESSEQUAL)); // D3DCMPFUNC
        glDepthFunc(GL_LEQUAL);
        break;
        
    case API(D3DRENDERSTATE_ALPHAFUNC):
        assert(b == API(D3DCMP_NOTEQUAL)); // D3DCMPFUNC
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_DITHERENABLE):
        assert(b == 1); // TRUE to enable dithering
        glSet(GL_DITHER, b);
        break;
        
    case API(D3DRENDERSTATE_ALPHABLENDENABLE):
        assert((b == 0) || (b == 1)); // TRUE to enable alpha blending
        glSet(GL_BLEND, b);
        break;
        
    case API(D3DRENDERSTATE_FOGENABLE):
        assert((b == 0) || (b == 1)); // TRUE to enable fog blending
        fogEnable = b;
        break;
        
    case API(D3DRENDERSTATE_SPECULARENABLE):
        assert(b == 0); // TRUE to enable specular
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_SUBPIXEL):
        assert(b == 1); // TRUE to enable subpixel correction
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_STIPPLEDALPHA):
        assert((b == 0) || (b == 1)); // TRUE to enable stippled alpha (RGB device only)
        // FIXME
        break;
        
    case API(D3DRENDERSTATE_FOGCOLOR):
        // D3DCOLOR
        fogColor = b;
        break;
        
    case API(D3DRENDERSTATE_FOGTABLEMODE):
        assert((b == API(D3DFOG_NONE)) || (b == API(D3DFOG_LINEAR))); // D3DFOGMODE
        fogMode = b;
        break;
        
    case API(D3DRENDERSTATE_FOGTABLESTART):
        // Fog start (for both vertex and pixel fog)
        fogStart = *(float*)&b;
        break;
        
    case API(D3DRENDERSTATE_FOGTABLEEND):
        // Fog end
        fogEnd = *(float*)&b;
        break;
        
    case API(D3DRENDERSTATE_COLORKEYENABLE):
        assert(b == 1); // TRUE to enable source colorkeyed textures
        // FIXME
        break;
        
    default:
        info_printf("Unknown render-state %d set to 0x%08" PRIX32 " (%f)\n", a, b, *(float*)&b);
        assert(false);
        break;
  }
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE,LPD3DMATRIX) PURE; // 25
HACKY_COM_BEGIN(IDirect3DDevice3, 25)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  uint32_t a = stack[2];
  float* m = (float*)Memory(stack[3]);
  switch(a) {
    case 3: // Projection
        memcpy(projectionMatrix, m, 4 * 4 * sizeof(float));
        break;
    default:
        info_printf("Unknown matrix %d\n", a);
        //FIXME: assert(false) once this runs faster
        break;
  }
  info_printf("Matrix %d:\n", a);
  info_printf("  %f\t%f\t%f\t%f\n", m[ 0], m[ 1], m[ 2], m[ 3]);
  info_printf("  %f\t%f\t%f\t%f\n", m[ 4], m[ 5], m[ 6], m[ 7]);
  info_printf("  %f\t%f\t%f\t%f\n", m[ 8], m[ 9], m[10], m[11]);
  info_printf("  %f\t%f\t%f\t%f\n", m[12], m[13], m[14], m[15]);

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()


// IDirect3DDevice3 -> STDMETHOD(DrawPrimitive)(THIS_ D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,DWORD) PURE; // 28
HACKY_COM_BEGIN(IDirect3DDevice3, 28)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a (primitive type) 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b (vertex format = fvf) 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c (buffer) 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("d (vertex-count) 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("e (flags) 0x%" PRIX32 "\n", stack[6]);

  PrintVertices(stack[3], stack[4], stack[5]);
  LoadVertices(stack[3], stack[4], stack[5]);
  GLenum mode = SetupRenderer(stack[2], stack[3]);
  glDrawArrays(mode, 0, stack[5]);

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 6 * 4;
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD(DrawIndexedPrimitive)(THIS_ D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,LPWORD,DWORD,DWORD) PURE; // 29
HACKY_COM_BEGIN(IDirect3DDevice3, 29)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("d3dptPrimitiveType 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("dwVertexTypeDesc 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("lpvVertices 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("dwVertexCount 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("lpwIndices 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("dwIndexCount 0x%" PRIX32 "\n", stack[7]);
  hacky_printf("dwFlags 0x%" PRIX32 "\n", stack[8]);

  LoadIndices(stack[6], stack[7]);
  LoadVertices(stack[3], stack[4], stack[5]);
  GLenum mode = SetupRenderer(stack[2], stack[3]);
  glDrawElements(mode, stack[7], GL_UNSIGNED_SHORT, NULL);

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 8 * 4;
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD(SetTexture)(THIS_ DWORD,LPDIRECT3DTEXTURE2) PURE; // 38
HACKY_COM_BEGIN(IDirect3DDevice3, 38)
  uint32_t a = stack[2];
  uint32_t b = stack[3];
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", a);
  hacky_printf("b 0x%" PRIX32 "\n", b);

  if (b != 0) {
    API(Direct3DTexture2)* texture = (API(Direct3DTexture2)*) Memory(b);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->handle);
  } else {
    glBindTexture(GL_TEXTURE_2D, 0); // FIXME: I believe this is supposed to be white?!
    assert(false);
  }

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirect3DDevice3 -> STDMETHOD(SetTextureStageState)(THIS_ DWORD,D3DTEXTURESTAGESTATETYPE,DWORD) PURE; // 40
HACKY_COM_BEGIN(IDirect3DDevice3, 40)
  uint32_t a = stack[2];
  uint32_t b = stack[3];
  uint32_t c = stack[4];

  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", a);
  hacky_printf("b 0x%" PRIX32 "\n", b);
  hacky_printf("c 0x%" PRIX32 "\n", c);

  assert(a == 0);

  switch(b) {
        
    case API(D3DTSS_ADDRESSU):
        assert((c == API(D3DTADDRESS_WRAP)) || (c == API(D3DTADDRESS_CLAMP))); // D3DTEXTUREADDRESS
        //FIXME
        break;
    case API(D3DTSS_ADDRESSV):
        assert((c == API(D3DTADDRESS_WRAP)) || (c == API(D3DTADDRESS_CLAMP))); // D3DTEXTUREADDRESS
        //FIXME
        break;
    case API(D3DTSS_MAGFILTER): // D3DTEXTUREMAGFILTER
        assert(c == API(D3DTFG_LINEAR));
        //FIXME
        break;
    case API(D3DTSS_MINFILTER): // D3DTEXTUREMINFILTER
        assert(c == API(D3DTFN_LINEAR));
        //FIXME
        break;
    case API(D3DTSS_MIPFILTER): // D3DTEXTUREMIPFILTER
        assert(c == API(D3DTFP_NONE));
        //FIXME
        break;
    default:
        assert(false);
        break;
  }

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 4 * 4;
HACKY_COM_END()


// IDirect3DTexture2

// IDirect3DTexture2 -> STDMETHOD(QueryInterface)                (THIS_ REFIID, LPVOID FAR *) PURE; // 0
HACKY_COM_BEGIN(IDirect3DTexture2, 0)
  hacky_printf("QueryInterface\n");
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

  if (!strcmp(iidString, "0B2B8630-AD35-11D0-8EA6-00609797EA5B")) {
    API(Direct3DTexture2)* This = (API(Direct3DTexture2)*)Memory(stack[1]);
    *(Address*)Memory(stack[3]) = This->surface;
  } else {
    assert(false);
  }

  eax = 0;
  esp += 3 * 4;
HACKY_COM_END()


// IDirect3DTexture2 -> STDMETHOD_(ULONG,Release)       (THIS) PURE; //2
HACKY_COM_BEGIN(IDirect3DTexture2, 2)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IDirect3DTexture2 -> STDMETHOD(GetHandle)(THIS_ LPDIRECT3DDEVICE2,LPD3DTEXTUREHANDLE) PURE; // 3
HACKY_COM_BEGIN(IDirect3DTexture2, 3)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  *(uint32_t*)Memory(stack[3]) = 1248;
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirect3DTexture2 -> STDMETHOD(Load)(THIS_ LPDIRECT3DTEXTURE2) PURE; // 5
HACKY_COM_BEGIN(IDirect3DTexture2, 5)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

  API(Direct3DTexture2)* This = (API(Direct3DTexture2)*) Memory(stack[1]);
  API(Direct3DTexture2)* a = (API(Direct3DTexture2)*) Memory(stack[2]);
  //FIXME: Dirty hack..
  This->handle = a->handle;
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()


// IDirect3DViewport3

// IDirect3DViewport3 -> STDMETHOD_(ULONG,Release)       (THIS) PURE; //2
HACKY_COM_BEGIN(IDirect3DViewport3, 2)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IDirect3DViewport3 -> STDMETHOD(SetViewport2)(THIS_ LPD3DVIEWPORT2) PURE; // 17
HACKY_COM_BEGIN(IDirect3DViewport3, 17)
  hacky_printf("this 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("pViewport 0x%" PRIX32 "\n", stack[2]);
  API(D3DVIEWPORT2)* vp = (API(D3DVIEWPORT2)*)Memory(stack[2]);
  assert(vp->dwSize == sizeof(API(D3DVIEWPORT2)));
  Game *game = reinterpret_cast<Game*>(_user_data);

  // Makesure works with high-DPI
  float scaleX = game ? game->hidpi_x() : 1.0;
  float scaleY = game ? game->hidpi_y() : 1.0;

  clipScale[0] = 2.0f / vp->dvClipWidth;
  clipScale[1] = 2.0f / vp->dvClipHeight;
  clipScale[2] = 2.0f / (vp->dvMaxZ - vp->dvMinZ);
  clipOffset[0] = -vp->dvClipX * clipScale[0] - 1.0f;
  clipOffset[1] = -vp->dvClipY * clipScale[1] - 1.0f;
  clipOffset[2] = -vp->dvMinZ * clipScale[2] - 1.0f;

  glViewport(vp->dwX * scaleX, vp->dwY * scaleY, vp->dwWidth * scaleX, vp->dwHeight * scaleY);

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()

// IDirect3DViewport3 -> STDMETHOD(Clear2)(THIS_ DWORD,LPD3DRECT,DWORD,D3DCOLOR,D3DVALUE,DWORD) PURE; // 20
HACKY_COM_BEGIN(IDirect3DViewport3, 20)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("d 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("e 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("f 0x%" PRIX32 "\n", stack[7]);

  unsigned int rectCount = stack[2];
  API(D3DRECT)* rects = (API(D3DRECT)*) Memory(stack[3]);

  glEnable(GL_SCISSOR_TEST);
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  for(unsigned int i = 0; i < rectCount; i++) {
    API(D3DRECT)* rect = &rects[i];
    //FIXME: Clip to viewport..
    int width = rect->x2 - rect->x1;
    int height = rect->y2 -  rect->y1;
    glScissor(rect->x1, viewport[3] - rect->y2, width, height);
    
    unsigned int flags = stack[4];
    uint32_t clearColor = stack[5];
    float zValue = *(float*)&stack[6];
    uint32_t stencilValue = stack[7];
    
    float a = (clearColor >> 24) / 255.0f;
    float r = ((clearColor >> 24) & 0xFF) / 255.0f;
    float g = ((clearColor >> 16) & 0xFF) / 255.0f;
    float b = (clearColor & 0xFF) / 255.0f;
    
    glClearStencil(stencilValue);
    glClearDepth(zValue);
    glClearColor(r, g, b, a);
    glClear(((flags & API(D3DCLEAR_TARGET)) ? GL_COLOR_BUFFER_BIT : 0) |
            ((flags & API(D3DCLEAR_ZBUFFER)) ? GL_DEPTH_BUFFER_BIT : 0) |
            ((flags & API(D3DCLEAR_STENCIL)) ? GL_STENCIL_BUFFER_BIT : 0));
  }
  glDisable(GL_SCISSOR_TEST);

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 7 * 4;
HACKY_COM_END()





