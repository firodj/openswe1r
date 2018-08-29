// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#ifndef __OPENSWE1R_EXPORT_H__
#define __OPENSWE1R_EXPORT_H__

#include <stdio.h>

#include "main.h"

#define EXPORT_STDCALL(library, returnType, symbol, ...) \
INITIALIZER(export_ ## library ## _ ## symbol) { \
  sys_printf("Exporting stdcall '%s' from '%s'\n", #symbol, #library); \
} \
returnType library ## _ ## symbol()

#endif
