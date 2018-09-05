// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "../main.h"
#include "../windows.h"
#include "../export.h"
#include "../emulation.h"
#include <string.h>
#include <assert.h>

#include "../Application.hpp"
#include "../Game.hpp"

extern Address wndMainProc;

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
  double x;
  double y;
  //GLFWwindow* glfwWindow = Application::Get()->window();

  //SDL_GetMouseState(&x, &y);
  //glfwGetCursorPos(glfwWindow, &x, &y);

  point[0] = (int)x;
  point[1] = (int)y;
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

#define API__WM_LBUTTONDOWN                  0x0201
#define API__WM_QUIT                         0x0012
#define API__WM_CLOSE                        0x0010
#define API__WM_DESTROY                      0x0002
#define API__PM_REMOVE                       0x0001
#define API__WM_TIMER                        0x0113

static API(MSG) pendingMsg = {0};

//User32.lib
HACKY_IMPORT_BEGIN2(PeekMessageA)
  //my_printf("PeekMessageA");
  //my_printf("  lpMsg:0x%" PRIX32, stack[1]);
  //my_printf("  hWnd:0x%" PRIX32, stack[2]);
  //my_printf("  wMsgFilterMin:0x%" PRIX32, stack[3]);
  //my_printf("  wMsgFilterMax:0x%" PRIX32, stack[4]);
  //my_printf("  wRemoveMsg:0x%" PRIX32 "\n", stack[5]);

  //GLFWwindow* glfwWindow = Application::Get()->window();

  API(MSG) *lpMsg = (API(MSG)*) Memory(stack[1]);
  eax = 0; // If a message is available, the return value is nonzero.

  //glfwPollEvents();

  if (0) { // glfwWindowShouldClose(glfwWindow)) {
    memset(lpMsg, 0x00, sizeof(API(MSG)));
    
    lpMsg->message = API(WM_DESTROY);
    lpMsg->hwnd = stack[2];
    
    pendingMsg = *lpMsg;
    eax = 1;
  } else {
    int state = 0; //glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS) {
      memset(lpMsg, 0x00, sizeof(API(MSG)));
      
      lpMsg->message = API(WM_LBUTTONDOWN);
      lpMsg->hwnd = stack[2];
      
      pendingMsg = *lpMsg;
      eax = 1;
    }
  }

  if (stack[5] == API(PM_REMOVE)) {
    memset(&pendingMsg, 0x00, sizeof(API(MSG)));memset(&pendingMsg, 0x00, sizeof(API(MSG)));
  }
HACKY_IMPORT_END2(5)

HACKY_IMPORT_BEGIN2(GetMessageA)
  my_printf("GetMessageA");
  my_printf("  lpMsg:0x%" PRIX32, stack[1]);
  my_printf("  hWnd:0x%" PRIX32, stack[2]);
  my_printf("  wMsgFilterMin:0x%" PRIX32, stack[3]);
  my_printf("  wMsgFilterMax:0x%" PRIX32 "\n", stack[4]);

  //GLFWwindow* glfwWindow = Application::Get()->window();
  
  API(MSG) *lpMsg = (API(MSG)*) Memory(stack[1]);

  //glfwPollEvents();

  if (pendingMsg.message) {
      *lpMsg = pendingMsg;
      
      memset(&pendingMsg, 0x00, sizeof(API(MSG)));
  } else {
      if (false) { // glfwWindowShouldClose(glfwWindow)) {
          memset(lpMsg, 0x00, sizeof(API(MSG)));
          
          lpMsg->message = API(WM_DESTROY);
          lpMsg->hwnd = stack[2];
      } else {

        int state = 0; //glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT);
        if (state == GLFW_PRESS) {
            memset(lpMsg, 0x00, sizeof(API(MSG)));
            
            lpMsg->message = API(WM_LBUTTONDOWN);
            lpMsg->hwnd = stack[2];
        }
    }

    eax = (lpMsg->message == API(WM_QUIT) || lpMsg->message == API(WM_DESTROY)) ? 0 : 1;
  }
HACKY_IMPORT_END2(4)

HACKY_IMPORT_BEGIN2(TranslateMessage)
  //my_printf("TranslateMessage");
  //my_printf("  lpMsg:0x%" PRIX32, stack[1]);

  // If the message is not translated (that is, a character message is not posted
  // to the thread's message queue), the return value is zero.
  eax = 0;
HACKY_IMPORT_END2(1)

HACKY_IMPORT_BEGIN2(DefWindowProcA)
  my_printf("DefWindowProcA");
  my_printf(" hWnd:0x%" PRIX32, stack[1]);
  my_printf(" Msg:0x%" PRIX32, stack[2]);
  my_printf(" wParam:0x%" PRIX32, stack[3]);
  my_printf(" lParam:0x%" PRIX32 "\n", stack[4]);
  eax = 0;
HACKY_IMPORT_END2(4)

HACKY_IMPORT_BEGIN2(PostQuitMessage)
  my_printf("PostQuitMessage");
  my_printf(" nExitCode:0x%" PRIX32 "\n", stack[1]);
 //PostQuitMessage function posts a WM_QUIT
HACKY_IMPORT_END2(1)

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
HACKY_IMPORT_BEGIN2(GetSystemMetrics)
  my_printf("GetSystemMetrics");
  my_printf(" nIndex:%" PRId32 "\n", stack[1]);
  Game *game = reinterpret_cast<Game*>(_user_data);
  switch(stack[1]) {
    case 0:
      eax = game ? game->window_width() : 640; // Horizontal resolution
      break;
    case 1:
      eax = game ? game->window_height() : 480; // Vertical resolution
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
  //esp += 1 * 4;
HACKY_IMPORT_END2(1)

//User32.lib
HACKY_IMPORT_BEGIN2(RegisterClassA)
  my_printf("RegisterClassA");
  my_printf("  lpWndClass:0x%" PRIX32 "\n", stack[1]);

  API(WNDCLASSA)* lpWndClass = (API(WNDCLASSA)*)Memory(stack[1]);
  my_printf(" .lpfnWndProc:0x%" PRIX32, lpWndClass->lpfnWndProc );

  if ((lpWndClass->lpszClassName & 0xFFFF000000)) {
    my_printf(" .lpszClassName:%s" PRIX32, (char*)Memory(lpWndClass->lpszClassName) );
  }
  my_printf("\n");
  eax = 444; //FIXME: ATOM, whatever that is?!
  //esp += 1 * 4;
HACKY_IMPORT_END2(1)

//User32.lib
HACKY_IMPORT_BEGIN2(RegisterClassExA)
  my_printf("RegisterClassExA");
  my_printf("  lpWndClassEx:0x%" PRIX32, stack[1]);

  API(WNDCLASSEXA)* lpWndClassEx = (API(WNDCLASSEXA)*)Memory(stack[1]);
  my_printf(" .lpfnWndProc:0x%" PRIX32, lpWndClassEx->lpfnWndProc );

  if ((lpWndClassEx->lpszClassName & 0xFFFF00000)) {
    my_printf(" .lpszClassName:%s" PRIX32, (char*)Memory(lpWndClassEx->lpszClassName) );
    *(uint32_t*)Memory(wndMainProc) = lpWndClassEx->lpfnWndProc;
  }

  my_printf("\n");
  eax = 444; //FIXME: ATOM, whatever that is?!
  //esp += 1 * 4;
HACKY_IMPORT_END2(1)

//User32.lib
HACKY_IMPORT_BEGIN2(CreateWindowExA)
  my_printf("CreateWindowExA");
  my_printf(" dwExStyle:0x%" PRIX32, stack[1]);
  my_printf(" lpClassName:0x%" PRIX32 " ('%s')", stack[2], (char*)Memory(stack[2]));
  my_printf(" lpWindowName:0x%" PRIX32 " ('%s')", stack[3], (char*)Memory(stack[3]));
  my_printf(" dwStyle:0x%" PRIX32, stack[4]);
  my_printf(" x:0x%" PRIX32, stack[5]);
  my_printf(" y:0x%" PRIX32, stack[6]);
  my_printf(" nWidth:0x%" PRIX32, stack[7]);
  my_printf(" nHeight:0x%" PRIX32, stack[8]);
  my_printf(" hWndParent:0x%" PRIX32, stack[9]);
  my_printf(" hMenu:0x%" PRIX32, stack[10]);
  my_printf(" hInstance:0x%" PRIX32, stack[11]);
  my_printf(" lpParam:0x%" PRIX32 "\n", stack[12]);
  eax = 333; // HWND
  //esp += 12 * 4;
HACKY_IMPORT_END2(12)

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
