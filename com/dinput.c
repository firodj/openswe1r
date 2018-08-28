// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

// Stolen from https://github.com/sekishi1259/SimpleROHook/blob/master/Injection/ProxyIDirectInput.h

#include "../main.h"
#include "../emulation.h"
#include "dinput.h"

#if 0

class CProxyIDirectInput7 : public IDirectInput7
{
private:
	IDirectInput7*			m_Instance;
public:
	CProxyIDirectInput7(IDirectInput7* ptr) : m_Instance(ptr) {}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID p1, LPVOID * p2) PROXY2(QueryInterface) //0
	STDMETHOD_(ULONG,AddRef) (THIS)		PROXY0(AddRef) // 1
	STDMETHOD_(ULONG,Release) (THIS)	PROXY_RELEASE // 2

	/*** IDirectInput2A methods ***/
	//
	STDMETHOD(CreateDevice)(THIS_ REFGUID rguid,LPDIRECTINPUTDEVICEA *lpIDD,LPUNKNOWN pUnkOuter) // 3
	{
		return Proxy_CreateDevice(rguid, lpIDD,pUnkOuter);
	}
	STDMETHOD(EnumDevices)(THIS_ DWORD p1,LPDIENUMDEVICESCALLBACKA p2,LPVOID p3,DWORD p4) PROXY4(EnumDevices) // 4
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID p1)                                 PROXY1(GetDeviceStatus) // 5
	STDMETHOD(RunControlPanel)(THIS_ HWND p1,DWORD p2)                           PROXY2(RunControlPanel) // 6
	STDMETHOD(Initialize)(THIS_ HINSTANCE p1,DWORD p2)                           PROXY2(Initialize) // 7
	STDMETHOD(FindDevice)(THIS_ REFGUID p1,LPCSTR p2,LPGUID p3)                  PROXY3(FindDevice) // 8

	/*** IDirectInput7A methods ***/
	STDMETHOD(CreateDeviceEx)(THIS_ REFGUID p1,REFIID p2,LPVOID *p3,LPUNKNOWN p4)        PROXY4(CreateDeviceEx) // 9

	//
	// Proxy Functions
	//
	HRESULT Proxy_CreateDevice(THIS_ REFGUID rguid,LPDIRECTINPUTDEVICEA *lpIDD,LPUNKNOWN pUnkOuter); // 10?
};
#undef CLASSNAME

#endif



#if 0


DECLARE_INTERFACE_(IDirectInputDeviceA, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE; // 0
    STDMETHOD_(ULONG,AddRef)(THIS) PURE; // 1
    STDMETHOD_(ULONG,Release)(THIS) PURE; // 2

    /*** IDirectInputDeviceA methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE; // 3
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA,LPVOID,DWORD) PURE; // 4
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE; // 6
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE; // 8
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE; // 10
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE; // 12
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD) PURE; // 14
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE; // 16
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
};


#endif









// DirectInput

HACKY_IMPORT_BEGIN(DirectInputCreateA)
  hacky_printf("hinst 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwVersion 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("lplpDirectInput 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("punkOuter 0x%" PRIX32 "\n", stack[4]);
  //FIXME: Unsure about most terminology / inner workings here
  *(Address*)Memory(stack[3]) = CreateInterface("IDirectInputA", 200);
  eax = 0; // HRESULT -> non-negative means success
  esp += 4 * 4;
HACKY_IMPORT_END()




// IDirectInputA

// IDirectInputA -> STDMETHOD_(ULONG,Release)       (THIS) PURE; //2
HACKY_COM_BEGIN(IDirectInputA, 2)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IDirectInputA -> STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEA *,LPUNKNOWN) PURE;
HACKY_COM_BEGIN(IDirectInputA, 3)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("rguid 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("lpIDD 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("pUnkOuter 0x%" PRIX32 "\n", stack[4]);
  *(Address*)Memory(stack[3]) = CreateInterface("IDirectInputDeviceA", 200);
  eax = 0; // HRESULT -> non-negative means success
  esp += 4 * 4;
HACKY_COM_END()

// IDirectInputA -> STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD) PURE;
HACKY_COM_BEGIN(IDirectInputA, 4)
  hacky_printf("EnumDevices\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  uint32_t a = stack[2];
  uint32_t b = stack[3];
  uint32_t c = stack[4];
  uint32_t d = stack[5];
  hacky_printf("a 0x%" PRIX32 "\n", a);
  hacky_printf("b 0x%" PRIX32 "\n", b);
  hacky_printf("c 0x%" PRIX32 "\n", c);
  hacky_printf("d 0x%" PRIX32 "\n", d);
  //FIXME: Do some callback stuff
  eax = 0; // HRESULT -> non-negative means success
  esp += 5 * 4;
  // Push a call to the callback onto the stack.. this is some ugly hack..

  // Convention is PASCAL

  esp -= 4;
  *(uint32_t*)Memory(esp) = returnAddress;

  {
    esp -= 4;
    *(uint32_t*)Memory(esp) = c; // pvRef

    Address ddiAddress = Allocate(sizeof(API(DIDEVICEINSTANCEA)));
    API(DIDEVICEINSTANCEA)* ddi = Memory(ddiAddress);
    memset(ddi, 0x00, sizeof(API(DIDEVICEINSTANCEA)));

    ddi->dwSize = sizeof(API(DIDEVICEINSTANCEA));
    //FIXME:    GUID guidInstance;
    //FIXME:    GUID guidProduct;
    enum {
      API(DIDEVTYPE_KEYBOARD) = 3
    };
    ddi->dwDevType = API(DIDEVTYPE_KEYBOARD); // or something
    sprintf(ddi->tszInstanceName, "OpenSWE1R Keyboard 1"); // TCHAR tszInstanceName[MAX_PATH];
    sprintf(ddi->tszProductName, "OpenSWE1R Keyboard"); // TCHAR tszProductName[MAX_PATH];
    //FIXME:    GUID guidFFDriver;
    ddi->wUsagePage = 0; //FIXME look at usb spec?
    ddi->wUsage = 0; //FIXME look at usb spec?

    esp -= 4;
    *(uint32_t*)Memory(esp) = ddiAddress; // LPCDIDEVICEINSTANCEA

    // Emulate the call
    esp -= 4;
    *(uint32_t*)Memory(esp) = clearEax; // Return to clear eax
    eip = b;

    info_printf("  Callback at 0x%" PRIX32 "\n", eip);
    //FIXME: Add a hook which returns 0
  }
HACKY_COM_END()





