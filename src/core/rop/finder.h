//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file core/rop/finder.h
/// Functions for scanning ELF binaries and detecting ROP gadgets.
///
//===----------------------------------------------------------------------===//

#ifndef CORE_ROP_FINDER_H
#define CORE_ROP_FINDER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stddef.h>
/// \brief Scans the .text section of an ELF binary for 'ret' instructions.
///
/// This function opens the ELF file specified by \p path, parses the header
/// to locate the .text section, and counts occurrences of the 'ret' opcode.
/// The \p context parameter specifies how many bytes of surrounding context
/// to consider for each gadget.
///
/// \param path Path to the ELF binary to scan.
/// \param context Number of surrounding bytes to include per gadget.
/// \return Returns the number of 'ret' instructions found, or -1 on error.
int find_ret_instructions(const char *path, int context_bytes);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CORE_ROP_FINDER_H
