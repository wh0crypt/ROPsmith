//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file core/ropsmith.cpp
/// Functions for scanning ELF binaries and detecting ROP gadgets.
///
//===----------------------------------------------------------------------===//

#include "ropsmith.h"

#include "rop.h"

#include <cstdio>

namespace ropsmith {

void print_info(void) {
    std::printf("=== ROPsmith ===\n");
    std::printf("Version: %s\n", VERSION.c_str());
    std::printf("Description: ROP gadget finder & chain generator.\n");
    std::printf("Build: %s %s\n", __DATE__, __TIME__);
    std::printf("================\n\n");
}

int scan_file(const char *path, int context_bytes) {
    int ret_count = rop::get_file_ret_count(path, context_bytes);
    return ret_count;
}

} // namespace ropsmith
