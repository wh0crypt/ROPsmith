//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file core/rop/scanner.h
/// Functions for scanning ELF binaries and detecting ROP gadgets.
///
//===----------------------------------------------------------------------===//

#ifndef CORE_ROP_SCANNER_H
#define CORE_ROP_SCANNER_H

#if defined(__APPLE__) || defined(_WIN32)
#include "include/elf.h"
#else
#include <elf.h>
#endif // ELF headers

#include "core/binary.h"
#include "macros.h"

#include <cstring>
#include <filesystem>
#include <stdexcept>

namespace rop {
/// \brief Prints information about a section.
///
/// \param offset Offset of the section in the file.
/// \param size Size of the section.
/// \param addr Virtual address of the section.
void print_section_info(uint64_t offset, uint64_t size, uint64_t addr);

/// \brief Scans the .text section of an ELF binary for 'ret' instructions.
///
/// This function opens the ELF file specified by \p path, parses the header
/// to locate the .text section, and counts occurrences of the 'ret' opcode.
/// The \p context parameter specifies how many bytes of surrounding context
/// to consider for each gadget.
///
/// \param path Path to the ELF binary file.
/// \param context Number of surrounding bytes to include per gadget.
/// \return Returns the number of 'ret' instructions found.
/// \throws std::runtime_error if the type is not supported, the file cannot be
/// accessed, or the file is empty.
std::size_t find_ret_instructions(const std::filesystem::path &path,
                                  int context_bytes = DEFAULT_CONTEXT_BYTES);

/// \brief Scans the .text section of an ELF binary for 'ret' instructions.
///
/// This function opens the ELF file specified by \p path, parses the header
/// to locate the .text section, and counts occurrences of the 'ret' opcode.
/// The \p context parameter specifies how many bytes of surrounding context
/// to consider for each gadget.
///
/// \tparam T Type representing the file.
/// \param context Number of surrounding bytes to include per gadget.
/// \return Returns the number of 'ret' instructions found.
/// \throws std::runtime_error if the type is not supported, the file cannot be
/// accessed, or the file is empty.
template <typename T>
std::size_t find_ret_instructions(const T &file,
                                  int context_bytes = DEFAULT_CONTEXT_BYTES) {
    if (file.type() != file::BinaryType::ELF) {
        throw std::runtime_error("unsupported file type (only ELF supported)");
    }

    std::size_t ret_count = 0;

    try {
        ret_count = find_ret_instructions(file.path(), context_bytes);
    } catch (const std::runtime_error &e) {
        throw;
    }

    return ret_count;
}

} // namespace rop

#endif // CORE_ROP_SCANNER_H
