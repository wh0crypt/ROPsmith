//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file core/rop.cpp
/// Implementations for scanning ELF binaries and detecting ROP
/// gadgets.
///
//===----------------------------------------------------------------------===//

#include "rop.h"

#include "rop/finder.h"

namespace rop {

int get_file_ret_count(const char *path, int context_bytes) {
    return find_ret_instructions(path, context_bytes);
}

} // namespace rop
