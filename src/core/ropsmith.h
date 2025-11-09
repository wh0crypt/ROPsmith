//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file core/ropsmith.h
/// Main public header for ROPsmith. Provides high-level functions and
/// utility functions for printing info and handling user interaction.
///
//===----------------------------------------------------------------------===//

#ifndef CORE_ROPSMITH_H
#define CORE_ROPSMITH_H

#include "macros.h"
#include "rop/scanner.h"

#include <cstddef>
#include <string>

namespace ropsmith {

/// Project-wide version constant
constexpr std::string VERSION = "0.1";

/// \brief Prints basic information about the tool.
///
/// This function prints the banner, version, or other relevant info to
/// the standard output. Can be called at the start of main().
void print_info(void);

/// \brief Scans the specified ELF file for ROP gadgets.
///
/// This is a high-level wrapper around the internal scanning functions.
/// \tparam T Type representing the file.
/// \param context_bytes Number of context bytes to include around each gadget.
/// \return Returns the number of 'ret' instructions found.
/// \throws std::runtime_error if the type is not supported, the file cannot be
/// accessed, or the file is empty.
template <typename T>
std::size_t scan_file(const T &file,
                      int      context_bytes = DEFAULT_CONTEXT_BYTES) {
    std::size_t ret_count = 0;

    try {
        ret_count = rop::find_ret_instructions(file, context_bytes);
    } catch (const std::runtime_error &e) {
        throw std::runtime_error(std::string("scan_file error: ") + e.what());
    }

    return ret_count;
}

} // namespace ropsmith

#endif // CORE_ROPSMITH_H
