// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "../main.h"
#include "../windows.h"
#include "../export.h"
#include "../emulation.h"
#include <string.h>
#include <assert.h>

#include "SDL.h"

#if 0

0x1C9 'MoveWindow'
0x112 'GetKeyState'
0x1B5 'MapVirtualKeyA'
0x2AC 'wsprintfA'
0x1A0 'LoadImageA'
0x146 'GetSystemMetrics'
0x15C 'GetWindowRect'
0x95 'DispatchMessageA'
0x282 'TranslateMessage'
0x1DC 'PeekMessageA'
0xD5 'FindWindowA'        <<<< first call ever in WinMain
0x226 'SetCursor'
0x291 'UpdateWindow'
0x26A 'ShowWindow'
0x59 'CreateWindowExA'
0x1F2 'RegisterClassA'
0x19A 'LoadCursorA'
0x19E 'LoadIconA'
0x1BE 'MessageBoxA'
0x1E0 'PostQuitMessage'
0x84 'DefWindowProcA'

#endif

// Actual revolt.exe starts here, anything until this point was CRT-Startup / pre-WinMain:

//User32.lib
HACKY_IMPORT_BEGIN(GetCursorPos)
  hacky_printf("lpPoint 0x%" PRIX32 "\n", stack[1]);
  int32_t* point = (int32_t*)Memory(stack[1]);
  int x;
  int y;
  SDL_GetMouseState(&x, &y);
  point[0] = x;
  point[1] = y;
  eax = 1; // nonzero if succeeds
  esp += 1 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(SetCursorPos)
  hacky_printf("x %" PRIu32 "\n", stack[1]);
  hacky_printf("y %" PRIu32 "\n", stack[2]);
  eax = 1; // nonzero if succeeds
  esp += 2 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(GetKeyNameTextA)
  hacky_printf("lParam 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpString 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));
  hacky_printf("cchSize %" PRIu32 "\n", stack[3]);
  eax = snprintf((char*)Memory(stack[2]), stack[3], "k%" PRIu32, stack[1]); // Cancel was selected
  esp += 3 * 4;
HACKY_IMPORT_END()

//User32.lib
  HACKY_IMPORT_BEGIN(FindWindowA)
  hacky_printf("lpClassName 0x%" PRIX32 " ('%s')\n", stack[1], (char*)Memory(stack[1]));
  hacky_printf("lpWindowName 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));
  eax = 0; // NULL = window not found, else HWND
  esp += 2 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(MessageBoxA)
  hacky_printf("hWnd 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpText 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));
  hacky_printf("lpCaption 0x%" PRIX32 " ('%s')\n", stack[3], (char*)Memory(stack[3]));
  hacky_printf("uType 0x%" PRIX32 "\n", stack[4]);
  //SDL_Delay(5000);
  eax = 2; // Cancel was selected
  esp += 4 * 4;
HACKY_IMPORT_END()



//User32.lib
HACKY_IMPORT_BEGIN(wsprintfA)
// __cdecl!
  hacky_printf("lpOut 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpFmt 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));

  char* out = (char*)Memory(stack[1]);
  const char* in = (char*)Memory(stack[2]);

  unsigned int count = 0;
  unsigned int stackIndex = 3;
  while(*in != '\0') {
    const char* nextPercent = strchr(in, '%');
    if (nextPercent == NULL) {
        count += sprintf(&out[count], in); // Copy rest of the input to output
        break;
    }
    unsigned int length = nextPercent - in;
    memcpy(&out[count], in, length);
    in += length;
    count += length;
    in = nextPercent + 1;
    char type = *in++;
    switch(type) {
        case 'c':
            count += sprintf(&out[count], "%c", stack[stackIndex++]);
            break;
        case 's':
            count += sprintf(&out[count], "%s", (char*)Memory(stack[stackIndex++]));
            break;
        case 'd':
            count += sprintf(&out[count], "%d", stack[stackIndex++]);
            break;
        default:
            info_printf("Unknown format type '%c'\n", type);
            assert(false);
    }
  }
  eax = count;

  info_printf("Out: '%s'\n", out);
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(PeekMessageA)
  hacky_printf("lpMsg 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("hWnd 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("wMsgFilterMin 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("wMsgFilterMax 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("wRemoveMsg 0x%" PRIX32 "\n", stack[5]);

  //FIXME: Translate SDL events to windows events
  SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
      info_printf("\n\nSDL Exit!\n");
      exit(EXIT_FAILURE);
			break;
    }
  }

  eax = 0; // If a message is available, the return value is nonzero.
  esp += 5 * 4;
HACKY_IMPORT_END()

// Window creation function
//User32.lib
HACKY_IMPORT_BEGIN(LoadIconA)
  hacky_printf("hInstance 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpIconName 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));
  eax = 0; // NULL, pretend we failed
  esp += 2 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(LoadCursorA)
  hacky_printf("hInstance 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpCursorName 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));
  eax = 0; // NULL, pretend we failed
  esp += 2 * 4;
HACKY_IMPORT_END()

//Gdi32.lib
HACKY_IMPORT_BEGIN(GetStockObject)
  hacky_printf("fnObject 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // NULL, pretend we failed
  esp += 1 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(GetSystemMetrics)
  hacky_printf("nIndex %" PRId32 "\n", stack[1]);
  switch(stack[1]) {
    case 0:
      eax = 640; // Horizontal resolution
      break;
    case 1:
      eax = 480; // Vertical resolution
      break;
    case 15:
      eax = 0; //FIXME
      break;
    case 32:
      eax = 0; //FIXME
      break;
    default:
      eax = 16;
      info_printf("Unknown metric\n");
      assert(false);
      break;
  }
  esp += 1 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(RegisterClassA)
  hacky_printf("lpWndClass 0x%" PRIX32 "\n", stack[1]);
  eax = 444; //FIXME: ATOM, whatever that is?!
  esp += 1 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(RegisterClassExA)
  hacky_printf("lpWndClass 0x%" PRIX32 "\n", stack[1]);
  eax = 444; //FIXME: ATOM, whatever that is?!
  esp += 1 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(CreateWindowExA)
  hacky_printf("dwExStyle 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpClassName 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));
  hacky_printf("lpWindowName 0x%" PRIX32 " ('%s')\n", stack[3], (char*)Memory(stack[3]));
  hacky_printf("dwStyle 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("x 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("y 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("nWidth 0x%" PRIX32 "\n", stack[7]);
  hacky_printf("nHeight 0x%" PRIX32 "\n", stack[8]);
  hacky_printf("hWndParent 0x%" PRIX32 "\n", stack[9]);
  hacky_printf("hMenu 0x%" PRIX32 "\n", stack[10]);
  hacky_printf("hInstance 0x%" PRIX32 "\n", stack[11]);
  hacky_printf("lpParam 0x%" PRIX32 "\n", stack[12]);
  eax = 333; // HWND
  esp += 12 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(SetWindowPos)
  hacky_printf("hWnd 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("hWndInsertAfter 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("X 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("Y 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("cx 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("cy 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("uFlags 0x%" PRIX32 "\n", stack[7]);
  eax = 1; // nonzero if succeeds
  esp += 7 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(ShowWindow)
  hacky_printf("hWnd 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("nCmdShow 0x%" PRIX32 "\n", stack[2]);
  eax = 0; // Previously hidden
  esp += 2 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(UpdateWindow)
  hacky_printf("hWnd 0x%" PRIX32 "\n", stack[1]);
  eax = 1; // nonzero if succeeds
  esp += 1 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(SetCursor)
  hacky_printf("hCursor 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // NULL = there was no previous cursor
  esp += 1 * 4;
HACKY_IMPORT_END()

// Name entry screen
//User32.lib
HACKY_IMPORT_BEGIN(GetKeyState)
  API(SHORT) pressed = 0x8000; // high order bit = pressed
  API(SHORT) toggled = 0x0001; // low order bit = toggled
  API(SHORT) returnValue = 0; // default: unpressed
  int nVirtKey = stack[1];
  switch(nVirtKey) {
    case API(VK_CAPITAL):
      returnValue = 0;
      break;
    default:
      break;
  }
  eax = returnValue;
  esp += 1 * 4;
HACKY_IMPORT_END()

//User32.lib
HACKY_IMPORT_BEGIN(MapVirtualKeyA)
  API(UINT) uCode = stack[1];
  API(UINT) uMapType = stack[2];

  hacky_printf("uCode 0x%" PRIX32 "\n", uCode);
  hacky_printf("uMapType 0x%" PRIX32 "\n", uMapType);

  API(UINT) returnValue = 0; // 0 = no map
  switch(uMapType) {
    case 1: // MAPVK_VSC_TO_VK: uCode is a scan code and is translated into a virtual-key code that does not distinguish between left- and right-hand keys. If there is no translation, the function returns 0.
      if (uCode == API(VK_LSHIFT) || uCode == API(VK_RSHIFT)) {
        returnValue = API(VK_SHIFT);
      } else if (uCode == API(VK_LCONTROL) || uCode == API(VK_RCONTROL)) {
        returnValue = API(VK_CONTROL);
      } else if (uCode == API(VK_LMENU) || uCode == API(VK_RMENU)) {
        returnValue = API(VK_MENU);
      } else {
        returnValue = uCode; // FIXME: is this okay?
      }
      break;
    case 2: // MAPVK_VK_TO_CHAR: uCode is a virtual-key code and is translated into an unshifted character value in the low-order word of the return value. Dead keys (diacritics) are indicated by setting the top bit of the return value. If there is no translation, the function returns 0.
      if (uCode >= 0x41 && uCode <= 0x5A) { // A to Z key
        returnValue = 'a' + uCode - 0x41;
      }
      //FIXME: Other symbols
      // Assume unmapped otherwise
      break;
    default:
      info_printf("Unknown key map mode in MapVirtualKeyA\n");
      assert(false);
      break;
  }
  eax = returnValue;
  esp += 2 * 4;
HACKY_IMPORT_END()
