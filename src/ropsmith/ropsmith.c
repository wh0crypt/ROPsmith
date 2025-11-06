//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file ropsmith/ropsmith.c
/// Functions for scanning ELF binaries and detecting ROP gadgets.
///
//===----------------------------------------------------------------------===//

#include "../include/ropsmith/ropsmith.h"

#include <stdio.h>

void ropsmith_print_info(void) {
    printf("=== ROPsmith ===\n");
    printf("Version: %s\n", ROPSMITH_VERSION);
    printf("Description: ROP gadget finder & chain generator.\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);
    printf("================\n\n");
}
