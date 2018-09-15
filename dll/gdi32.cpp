// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "../main.h"
#include "../ms_windows.h"
#include "../export.h"
#include "../emulation.h"
#include "glad/glad.h"
#include <string.h>
#include <assert.h>

//Gdi32.lib
HACKY_IMPORT_BEGIN(GetObjectA)
  hacky_printf("hgdiobj 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("cbBuffer 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("lpvObject 0x%" PRIX32 "\n", stack[3]);
  //If the function succeeds, and lpvObject is a valid pointer, the return value is the number of bytes stored into the buffer.
  //If the function succeeds, and lpvObject is NULL, the return value is the number of bytes required to hold the information the function would store into the buffer.
  //If the function fails, the return value is zero.

  // Copy BITMAP from memory
  memcpy(Memory(stack[3]), Memory(stack[1]), stack[2]);

  eax = 0; // int
  esp += 3 * 4;
HACKY_IMPORT_END()

//Gdi32.lib
HACKY_IMPORT_BEGIN(CreateCompatibleDC)
  hacky_printf("hdc 0x%" PRIX32 "\n", stack[1]);
  // Hack: This is large enough to fit a pointer to the object (SelectObject)
  eax = Allocate(sizeof(Address));
  esp += 1 * 4;
HACKY_IMPORT_END()

//Gdi32.lib
HACKY_IMPORT_BEGIN(SelectObject)
  hacky_printf("hdc 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("hgdiobj 0x%" PRIX32 "\n", stack[2]);
  // Ugly hack.. let DC point at object
  *(Address*)Memory(stack[1]) = stack[2];
  eax = 0; //FIXME: It's complicated.. see docs instead!
  esp += 2 * 4;
HACKY_IMPORT_END()

//Gdi32.lib
HACKY_IMPORT_BEGIN(StretchBlt)
  hacky_printf("hdcDest 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("nXOriginDest 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("nYOriginDest 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("nWidthDest 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("nHeightDest 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("hdcSrc 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("nXOriginSrc 0x%" PRIX32 "\n", stack[7]);
  hacky_printf("nYOriginSrc 0x%" PRIX32 "\n", stack[8]);
  hacky_printf("nWidthSrc 0x%" PRIX32 "\n", stack[9]);
  hacky_printf("nHeightSrc 0x%" PRIX32 "\n", stack[10]);
  hacky_printf("dwRop 0x%" PRIX32 "\n", stack[11]);

  // Get the pointer to the object the DC points at, we'll assume that it is a BITMAP
  Address objectAddress = *(Address*)Memory(stack[6]);
  API(BITMAP)* bitmap = (API(BITMAP)*) Memory(objectAddress);
  void* data = Memory(bitmap->bmBits);

  // Update the texture interface
  if (stack[1] != 0) {
    GLint previousTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
    glBindTexture(GL_TEXTURE_2D, stack[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, stack[4], stack[5], 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, previousTexture);
  } else {


    info_printf("\n\n\n\nblt!!!!!!\n\n\n\n");
    glClearColor(1.0f,0.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    info_printf("Invalid destination!\n");
  }

  eax = 1; //  succeeds = return value is nonzero.
  esp += 11 * 4;
HACKY_IMPORT_END()

//Gdi32.lib
HACKY_IMPORT_BEGIN(DeleteDC)
  hacky_printf("hdc 0x%" PRIX32 "\n", stack[1]);
  eax = 1; //  succeeds = return value is nonzero
  esp += 1 * 4;
HACKY_IMPORT_END()

//Gdi32.lib
HACKY_IMPORT_BEGIN(DeleteObject)
  hacky_printf("hObject 0x%" PRIX32 "\n", stack[1]);
  eax = 1; //  succeeds = return value is nonzero
  esp += 1 * 4;
HACKY_IMPORT_END()

//Gdi32.lib
HACKY_IMPORT_BEGIN(GetPixel)
  int nXPos = stack[2];
  int nYPos = stack[3];
  // Heavily impacts performance!
#if 0
  hacky_printf("hdc 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("nXPos 0x%" PRIX32 "\n", nXPos);
  hacky_printf("nYPos 0x%" PRIX32 "\n", nYPos);
#else
  silent = true;
#endif
  eax = 0xFF00FF00 ^ nXPos ^ nYPos; //  Color
  esp += 3 * 4;
HACKY_IMPORT_END()

// Weird font stuff
//Gdi32.lib
HACKY_IMPORT_BEGIN(CreateFontA)
  hacky_printf("nHeight 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("nWidth 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("nEscapement 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("nOrientation 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("fnWeight 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("fdwItalic 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("fdwUnderline 0x%" PRIX32 "\n", stack[7]);
  hacky_printf("fdwStrikeOut 0x%" PRIX32 "\n", stack[8]);
  hacky_printf("fdwCharSet 0x%" PRIX32 "\n", stack[9]);
  hacky_printf("fdwOutputPrecision 0x%" PRIX32 "\n", stack[10]);
  hacky_printf("fdwClipPrecision 0x%" PRIX32 "\n", stack[11]);
  hacky_printf("fdwQuality 0x%" PRIX32 "\n", stack[12]);
  hacky_printf("fdwPitchAndFamily 0x%" PRIX32 "\n", stack[13]);
  const char* lpszFace = (const char*)Memory(stack[14]);
  hacky_printf("lpszFace 0x%" PRIX32 " ('%s')\n", stack[14], lpszFace);
  eax = 0x1337C0DE; // HFONT Handle
  esp += 14 * 4;
HACKY_IMPORT_END()
