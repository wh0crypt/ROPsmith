//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file core/rop.h
/// Declarations for scanning ELF binaries and detecting ROP
/// gadgets.
///
//===----------------------------------------------------------------------===//

#ifndef CORE_ROP_H
#define CORE_ROP_H

namespace rop {

/// \brief Get the count of 'ret' instructions in the specified ELF file.
///
/// This function is a high-level wrapper around the internal scanning
/// functions.
///
/// \param path Path to the ELF binary to scan.
/// \param context_bytes Number of surrounding bytes to include per gadget.
/// \return Returns the number of 'ret' instructions found, or -1 on error.
int get_file_ret_count(const char *path, int context_bytes);

} // namespace rop

#endif // CORE_ROP_H
