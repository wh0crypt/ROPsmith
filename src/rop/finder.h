//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file rop/finder.h
/// Functions for scanning ELF binaries and detecting ROP gadgets.
///
//===----------------------------------------------------------------------===//

#ifndef ROP_FINDER_H
#define ROP_FINDER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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
int scan_elf_text_for_rets(const char* path, int context_bytes);

#ifdef __cplusplus
}
#endif

#endif // ROP_FINDER_H
