// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "../main.h"
#include "../windows.h"
#include "../export.h"
#include "../emulation.h"
#include <string.h>
#include <assert.h>

//Advapi32.lib
HACKY_IMPORT_BEGIN(RegCreateKeyExA)
  hacky_printf("hKey 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpSubKey 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));
  hacky_printf("Reserved 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("lpClass 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("dwOptions 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("samDesired 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("lpSecurityAttributes 0x%" PRIX32 "\n", stack[7]);
  hacky_printf("phkResult 0x%" PRIX32 "\n", stack[8]);
  hacky_printf("lpdwDisposition 0x%" PRIX32 "\n", stack[9]);
  *(uint32_t*)Memory(stack[8]) = 222; //FIXME: Pointer!
//    eax = 0; // ERROR_SUCCESS
  eax = 1; // anything which is not ERROR_SUCCESS
  esp += 9 * 4;
HACKY_IMPORT_END()

//Advapi32.lib
HACKY_IMPORT_BEGIN(RegQueryValueExA)
  hacky_printf("hKey 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpValueName 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));
  hacky_printf("lpReserved 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("lpType 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("lpData 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("lpcbData 0x%" PRIX32 "\n", stack[6]);

  // Patch to accept the CD
  if (!strcmp((char*)Memory(stack[2]), "CD Path")) {
    //FIXME: Assert that there is enough room for the path
    strcpy((char*)Memory(stack[5]), "D:");
    eax = 0; // ERROR_SUCCESS
  } else {
    eax = 2; // ERROR_FILE_NOT_FOUND
  }
  esp += 6 * 4;
HACKY_IMPORT_END()

//Advapi32.lib
HACKY_IMPORT_BEGIN(RegSetValueExA)
  hacky_printf("hKey 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpValueName 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));
  hacky_printf("Reserved 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("dwType 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("lpData 0x%" PRIX32 " ('%.*s')\n", stack[5], stack[6], (char*)Memory(stack[5]));
  hacky_printf("cbData 0x%" PRIX32 "\n", stack[6]);
  eax = 0; // ERROR_SUCCESS
  esp += 6 * 4;
HACKY_IMPORT_END()

//Advapi32.lib
HACKY_IMPORT_BEGIN(RegCloseKey)
  hacky_printf("hKey 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // ERROR_SUCCESS
  esp += 1 * 4;
HACKY_IMPORT_END()

//Advapi32.lib
HACKY_IMPORT_BEGIN(GetUserNameA)
  hacky_printf("lpBuffer 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpnSize 0x%" PRIX32 "\n", stack[2]);
  const char* name = "OpenSWE1R";
  assert(stack[2] >= (strlen(name) + 1));
  sprintf((char*)Memory(stack[1]), "%s", name);
  eax = 1; // BOOL -> success = nonzero
  esp += 2 * 4;
HACKY_IMPORT_END()

// Copy protection
//Advapi32.lib
HACKY_IMPORT_BEGIN(RegOpenKeyExA)
  hacky_printf("hKey 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpSubKey 0x%" PRIX32 " ('%s')\n", stack[2], (char*)Memory(stack[2]));
  hacky_printf("ulOptions 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("samDesired 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("phkResult 0x%" PRIX32 "\n", stack[5]);
  eax = 0; // ERROR_SUCCESS
  esp += 5 * 4;
HACKY_IMPORT_END()

