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

#include <string>

namespace ropsmith {

/// Project-wide version constant
const std::string VERSION = "0.1";

/// \brief Prints basic information about the tool.
///
/// This function prints the banner, version, or other relevant info to
/// the standard output. Can be called at the start of main().
void print_info(void);

/// \brief Scans the specified ELF file for ROP gadgets.
///
/// This is a high-level wrapper around the internal scanning functions.
/// \param path Path to the ELF binary to scan.
/// \param context_bytes Number of context bytes to include around each gadget.
/// \return Returns the number of 'ret' instructions found, or -1 on error.
int scan_file(const char *path, int context_bytes);

} // namespace ropsmith

#endif // CORE_ROPSMITH_H
