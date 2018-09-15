// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "../main.h"
#ifndef _WIN32
  #include "../ms_windows.h"
#else
  #include <windows.h>
#endif
#include "../export.h"
#include "../emulation.h"
#include <string.h>
#include <assert.h>

//Ole32.lib
HACKY_IMPORT_BEGIN(CoInitialize)
  hacky_printf("pvReserved 0x%" PRIX32 "\n", stack[1]);
  assert(stack[1] == 0x00000000);
  eax = 0; // S_OK
  esp += 1 * 4;
HACKY_IMPORT_END()

//Ole32.lib
HACKY_IMPORT_BEGIN(CoUninitialize)
  eax = 0; // void
  esp += 0 * 4;
HACKY_IMPORT_END()

//Ole32.lib
HACKY_IMPORT_BEGIN(CoCreateInstance)
  hacky_printf("rclsid 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("pUnkOuter 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("dwClsContext 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("riid 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("ppv 0x%" PRIX32 "\n", stack[5]);
  const API(CLSID)* clsid = (const API(CLSID)*)Memory(stack[1]);
  char clsidString[1024];
  sprintf(clsidString, "%08" PRIX32 "-%04" PRIX16 "-%04" PRIX16 "-%02" PRIX8 "%02" PRIX8 "-%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8,
          clsid->Data1, clsid->Data2, clsid->Data3,
          clsid->Data4[0], clsid->Data4[1], clsid->Data4[2], clsid->Data4[3],
          clsid->Data4[4], clsid->Data4[5], clsid->Data4[6], clsid->Data4[7]);
  info_printf("  (read clsid: {%s})\n", clsidString);
  const API(IID)* iid = (const API(IID)*)Memory(stack[4]);
  char iidString[1024];
  sprintf(iidString, "%08" PRIX32 "-%04" PRIX16 "-%04" PRIX16 "-%02" PRIX8 "%02" PRIX8 "-%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8,
          iid->Data1, iid->Data2, iid->Data3,
          iid->Data4[0], iid->Data4[1], iid->Data4[2], iid->Data4[3],
          iid->Data4[4], iid->Data4[5], iid->Data4[6], iid->Data4[7]);
  info_printf("  (read iid: {%s})\n", iidString);
  //FIXME: Unsure about most terminology / inner workings here
  static unsigned int comIndex = 0;
  char name[32];
  //FIXME: Add more classed / interfaces

  if (!strcmp(clsidString, "2FE8F810-B2A5-11D0-A787-0000F803ABFC")) {
    // DEFINE_GUID(CLSID_DirectPlayLobby, 0x2fe8f810, 0xb2a5, 0x11d0, 0xa7, 0x87, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc);
    if (!strcmp(iidString, "2DB72491-652C-11D1-A7A8-0000F803ABFC")) {
      strcpy(name, "IDirectPlayLobby3A");
    } else {
      assert(false);
    }
  } else if (!strcmp(clsidString, "D7B70EE0-4340-11CF-B063-0020AFC2CD35")) {
    // DEFINE_GUID(CLSID_DirectDraw, 0xD7B70EE0,0x4340,0x11CF,0xB0,0x63,0x00,0x20,0xAF,0xC2,0xCD,0x35);
    if (!strcmp(iidString, "9C59509A-39BD-11D1-8C4A-00C04FD930C5")) {
      strcpy(name, "IDirectDraw4");
    } else {
      assert(false);
    }
  } else if (!strcmp(clsidString, "92FA2C24-253C-11D2-90FB-006008A1F441")) {
    if (!strcmp(iidString, "E4C40280-CCBA-11D2-9DCF-00500411582F")) {
      strcpy(name, "IA3d4");
    } else {
      assert(false);
    }
  } else if (!strcmp(clsidString, "D1EB6D20-8923-11D0-9D97-00A0C90A43CB")) {
    if (!strcmp(iidString, "0AB1C530-4745-11D1-A7A1-0000F803ABFC")) {
      strcpy(name, "IDirectPlay4");
    } else {
      assert(false);
    }
  } else {
    assert(false);
  }
  *(Address*)Memory(stack[5]) = CreateInterface(name, 200, 100);
  comIndex++;
  eax = 0; // S_OK
  esp += 5 * 4;
HACKY_IMPORT_END()


