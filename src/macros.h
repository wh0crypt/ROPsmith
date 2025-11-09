//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file macros.h
/// Macros and constants for ROP gadget detection.
///
//===----------------------------------------------------------------------===//

#ifndef MACROS_H
#define MACROS_H

#include <cstddef>

// Default values
constexpr int DEFAULT_CONTEXT_BYTES  = 16;
constexpr int DEFAULT_BYTES_PER_LINE = 16;

// Instruction opcodes
constexpr std::byte RET_OPCODE = std::byte{0xC3};

#endif // MACROS_H
