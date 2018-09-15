// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#ifndef __OPENSWE1R_MS_WINDOWS_H__
#define __OPENSWE1R_MS_WINDOWS_H__

#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "emulation.h"

    
typedef uint32_t API(DWORD);
typedef void API(VOID); // Beware of void*, shoulde be as Address

typedef int32_t API(LONG); // FIXME: Assumption
typedef uint32_t API(UINT); // FIXME: Assumption
typedef int16_t API(SHORT); // FIXME: Assumption


typedef struct {
  uint32_t Data1;
  uint16_t Data2;
  uint16_t Data3;
  uint8_t Data4[8];
} API(GUID);
typedef API(GUID) API(CLSID);
typedef API(GUID) API(IID);

typedef struct {
  uint32_t bmType;
  uint32_t bmWidth;
  uint32_t bmHeight;
  uint32_t bmWidthBytes;
  uint16_t bmPlanes;
  uint16_t bmBitsPixel;
  Address bmBits;
} API(BITMAP);

typedef API(DWORD) API(COLORREF);

typedef uint32_t API(SIZE_T);

typedef struct {
  API(DWORD) dwLowDateTime;
  API(DWORD) dwHighDateTime;
} API(FILETIME);

#define API__MAX_PATH 260

typedef char API(TCHAR);
typedef struct {
  API(DWORD)    dwFileAttributes;
  API(FILETIME) ftCreationTime;
  API(FILETIME) ftLastAccessTime;
  API(FILETIME) ftLastWriteTime;
  API(DWORD)    nFileSizeHigh;
  API(DWORD)    nFileSizeLow;
  API(DWORD)    dwReserved0;
  API(DWORD)    dwReserved1;
  API(TCHAR)    cFileName[API(MAX_PATH)];
  API(TCHAR)    cAlternateFileName[14];
} API(WIN32_FIND_DATA);

typedef struct {
  int x;
} API(STR);
typedef Address API(LPTSTR);


enum {
  API(VK_SHIFT) = 0x10,
  API(VK_CONTROL) = 0x11,
  API(VK_MENU) = 0x12,

  API(VK_CAPITAL) = 0x14,

  API(VK_LSHIFT) = 0xA0,
  API(VK_RSHIFT) = 0xA1,
  API(VK_LCONTROL) = 0xA2,
  API(VK_RCONTROL) = 0xA3,
  API(VK_LMENU) = 0xA4,
  API(VK_RMENU) = 0xA5
};

typedef struct tagPOINT {
    API(LONG) x;
    API(LONG) y;
} API(POINT);

typedef struct tagMSG {
    uint32_t   hwnd;
    API(UINT)   message;
    uint32_t wParam;
    uint32_t lParam;
    API(DWORD)  time;
    API(POINT)  pt;
    API(DWORD)  lPrivate;
} API(MSG);

typedef struct tagWNDCLASSA {
  API(UINT)  style;
  Address    lpfnWndProc;
  int32_t    cbClsExtra;
  int32_t    cbWndExtra;
  uint32_t   hInstance;
  uint32_t   hIcon;
  uint32_t   hCursor;
  uint32_t   hbrBackground;
  Address    lpszMenuName;
  Address    lpszClassName;
} API(WNDCLASSA);

typedef struct tagWNDCLASSEXA {
  API(UINT)  cbSize;
  API(UINT)  style;
  Address     lpfnWndProc;
  int32_t    cbClsExtra;
  int32_t    cbWndExtra;
  uint32_t   hInstance;
  uint32_t   hIcon;
  uint32_t   hCursor;
  uint32_t   hbrBackground;
  Address    lpszMenuName;
  Address    lpszClassName;
  uint32_t   hIconSm;
} API(WNDCLASSEXA);

#define API__CREATE_SUSPENDED  0x00000004

static int sprintf_ucs2(uint16_t* str, const char* fmt, ...) {
  va_list args;

  // Get string length
  va_start(args, fmt);
  int ret = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  // Copy string
  char* tmp = (char*)malloc(ret + 1);
  va_start(args, fmt);
  ret = vsprintf(tmp, fmt, args);
  va_end(args);
  for(unsigned int i = 0; i <= ret; i++) {
    str[i] = tmp[i];
  }
  free(tmp);

  return ret;
}

static void strcpy_ucs2(uint16_t* dest, uint16_t* src) {
  do {
    *dest++ = *src;
  } while(*src++ != 0);
}
    
#endif // __OPENSWE1R_MS_WINDOWS_H__
