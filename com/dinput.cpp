// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

// Stolen from https://github.com/sekishi1259/SimpleROHook/blob/master/Injection/ProxyIDirectInput.h

#include "../main.h"
#include "../emulation.h"
#include "dinput.h"
#include <string.h>
#include <assert.h>

#include "SDL.h"

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
  *(Address*)Memory(stack[3]) = CreateInterface("IDirectInputA", 200, 100);
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
  *(Address*)Memory(stack[3]) = CreateInterface("IDirectInputDeviceA", 200, 100);
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
    API(DIDEVICEINSTANCEA)* ddi = (API(DIDEVICEINSTANCEA)*) Memory(ddiAddress);
    memset((void*)ddi, 0x00, sizeof(API(DIDEVICEINSTANCEA)));

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


// IDirectInputDeviceA

// IDirectInputDeviceA -> STDMETHOD_(ULONG,Release)       (THIS) PURE; //2
HACKY_COM_BEGIN(IDirectInputDeviceA, 2)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IDirectInputDeviceA -> STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE; // 3
HACKY_COM_BEGIN(IDirectInputDeviceA, 3)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

  //FIXME!

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 2 * 4;
HACKY_COM_END()

// IDirectInputDeviceA -> STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE; // 6
HACKY_COM_BEGIN(IDirectInputDeviceA, 6)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);

  //FIXME!

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirectInputDeviceA -> STDMETHOD(Acquire)(THIS) PURE; // 7
HACKY_COM_BEGIN(IDirectInputDeviceA, 7)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 1 * 4;
HACKY_COM_END()

// IDirectInputDeviceA -> STDMETHOD(Unacquire)(THIS) PURE; // 8
HACKY_COM_BEGIN(IDirectInputDeviceA, 8)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // HRESULT -> non-negative means success
  esp += 1 * 4;
HACKY_COM_END()

uint8_t keyboardState[256];
void UpdateKeyboardState() {
  const Uint8 *sdlState = SDL_GetKeyboardState(NULL);
  const uint8_t pressed = 0x80; // This is the only requirement for pressed keys
  const uint8_t unpressed = 0x00;
  memset(keyboardState, 0x00, 256);
  keyboardState[API(DIK_ESCAPE)] = sdlState[SDL_SCANCODE_ESCAPE] ? pressed : unpressed;
  keyboardState[API(DIK_RETURN)] = sdlState[SDL_SCANCODE_RETURN] ? pressed : unpressed;
  keyboardState[API(DIK_SPACE)] = sdlState[SDL_SCANCODE_SPACE] ? pressed : unpressed;
  keyboardState[API(DIK_UP)] = sdlState[SDL_SCANCODE_UP] ? pressed : unpressed;
  keyboardState[API(DIK_DOWN)] = sdlState[SDL_SCANCODE_DOWN] ? pressed : unpressed;
  keyboardState[API(DIK_LEFT)] = sdlState[SDL_SCANCODE_LEFT] ? pressed : unpressed;
  keyboardState[API(DIK_RIGHT)] = sdlState[SDL_SCANCODE_RIGHT] ? pressed : unpressed;
  keyboardState[API(DIK_Q)] = sdlState[SDL_SCANCODE_Q] ? pressed : unpressed;
  keyboardState[API(DIK_W)] = sdlState[SDL_SCANCODE_W] ? pressed : unpressed;
  keyboardState[API(DIK_E)] = sdlState[SDL_SCANCODE_E] ? pressed : unpressed;
  keyboardState[API(DIK_R)] = sdlState[SDL_SCANCODE_R] ? pressed : unpressed;
  keyboardState[API(DIK_I)] = sdlState[SDL_SCANCODE_I] ? pressed : unpressed;
  keyboardState[API(DIK_A)] = sdlState[SDL_SCANCODE_A] ? pressed : unpressed;
  keyboardState[API(DIK_S)] = sdlState[SDL_SCANCODE_S] ? pressed : unpressed;
  keyboardState[API(DIK_D)] = sdlState[SDL_SCANCODE_D] ? pressed : unpressed;
  keyboardState[API(DIK_F)] = sdlState[SDL_SCANCODE_F] ? pressed : unpressed;
  keyboardState[API(DIK_J)] = sdlState[SDL_SCANCODE_J] ? pressed : unpressed;
  keyboardState[API(DIK_K)] = sdlState[SDL_SCANCODE_K] ? pressed : unpressed;
  keyboardState[API(DIK_L)] = sdlState[SDL_SCANCODE_L] ? pressed : unpressed;
  keyboardState[API(DIK_M)] = sdlState[SDL_SCANCODE_M] ? pressed : unpressed;
  keyboardState[API(DIK_F1)] = sdlState[SDL_SCANCODE_F1] ? pressed : unpressed;
  keyboardState[API(DIK_F2)] = sdlState[SDL_SCANCODE_F2] ? pressed : unpressed;
  keyboardState[API(DIK_F3)] = sdlState[SDL_SCANCODE_F3] ? pressed : unpressed;
  keyboardState[API(DIK_F4)] = sdlState[SDL_SCANCODE_F4] ? pressed : unpressed;
  keyboardState[API(DIK_F5)] = sdlState[SDL_SCANCODE_F5] ? pressed : unpressed;
  keyboardState[API(DIK_F6)] = sdlState[SDL_SCANCODE_F6] ? pressed : unpressed;
  keyboardState[API(DIK_F7)] = sdlState[SDL_SCANCODE_F7] ? pressed : unpressed;
  keyboardState[API(DIK_F12)] = sdlState[SDL_SCANCODE_F12] ? pressed : unpressed;
  keyboardState[API(DIK_GRAVE)] = sdlState[SDL_SCANCODE_GRAVE] ? pressed : unpressed;
  keyboardState[API(DIK_EQUALS )] = sdlState[SDL_SCANCODE_EQUALS] ? pressed : unpressed;
  keyboardState[API(DIK_MINUS)] = sdlState[SDL_SCANCODE_MINUS] ? pressed : unpressed;
  keyboardState[API(DIK_TAB)] = sdlState[SDL_SCANCODE_TAB] ? pressed : unpressed;
  keyboardState[API(DIK_CAPSLOCK)] = sdlState[SDL_SCANCODE_CAPSLOCK] ? pressed : unpressed;
  keyboardState[API(DIK_LSHIFT)] = sdlState[SDL_SCANCODE_LSHIFT] ? pressed : unpressed;
  keyboardState[API(DIK_RSHIFT)] = sdlState[SDL_SCANCODE_RSHIFT] ? pressed : unpressed;
  keyboardState[API(DIK_LCONTROL)] = sdlState[SDL_SCANCODE_LCTRL] ? pressed : unpressed;
  keyboardState[API(DIK_RCONTROL)] = sdlState[SDL_SCANCODE_RCTRL] ? pressed : unpressed;
  keyboardState[API(DIK_LALT)] = sdlState[SDL_SCANCODE_LALT] ? pressed : unpressed;
  keyboardState[API(DIK_RALT)] = sdlState[SDL_SCANCODE_RALT] ? pressed : unpressed;
}

// IDirectInputDeviceA -> STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE; // 9
HACKY_COM_BEGIN(IDirectInputDeviceA, 9)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  UpdateKeyboardState();
  memcpy(Memory(stack[3]), keyboardState, stack[2]);
  eax = 0; // FIXME: No idea what this expects to return..
  esp += 3 * 4;
HACKY_COM_END()

// IDirectInputDeviceA -> STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE; // 10
HACKY_COM_BEGIN(IDirectInputDeviceA, 10)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("d 0x%" PRIX32 "\n", stack[5]);

  // Don't allow PEEK flag
  assert(stack[5] == 0);

  // Diff the keyboard input between calls
  static uint8_t previousState[256] = {0};
  assert(sizeof(previousState) == sizeof(keyboardState));
  UpdateKeyboardState();
  uint32_t* count = (uint32_t*)Memory(stack[4]);
  unsigned int max_count = *count;
  info_printf("max count is %d\n", max_count);
  *count = 0;
  unsigned int objectSize = stack[2];
  assert(objectSize == sizeof(API(DIDEVICEOBJECTDATA)));
  for(unsigned int i = 0; i < 256; i++) {
    if (keyboardState[i] != previousState[i]) {
      if (*count < max_count) {
        API(DIDEVICEOBJECTDATA) objectData;
        memset(&objectData, 0x00, sizeof(objectData));
        objectData.dwOfs = i;
        objectData.dwData = keyboardState[i];
        info_printf("Adding %d: %d\n", objectData.dwOfs, objectData.dwData);
        memcpy(Memory(stack[3] + *count * objectSize), &objectData, objectSize);
        *count = *count + 1;
      }
    }
  }
  memcpy(previousState, keyboardState, sizeof(keyboardState));
  info_printf("returning %d entries\n", *count);

  eax = 0; // FIXME: No idea what this expects to return..
  esp += 5 * 4;
HACKY_COM_END()

// IDirectInputDeviceA -> STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
HACKY_COM_BEGIN(IDirectInputDeviceA, 11)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0; // HRESULT -> non-negative means success
  esp += 2 * 4;
HACKY_COM_END()

// IDirectInputDeviceA -> STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
HACKY_COM_BEGIN(IDirectInputDeviceA, 13)
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  eax = 0; // HRESULT -> non-negative means success
  esp += 3 * 4;
HACKY_COM_END()




