//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file ropsmith/ropsmith.h
/// Main public header for ROPsmith. Provides high-level functions and
/// utility functions for printing info and handling user interaction.
///
//===----------------------------------------------------------------------===//

#ifndef ROPSMITH_H
#define ROPSMITH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Project-wide version constant
#define ROPSMITH_VERSION "0.1"

/// \brief Prints basic information about the ROPsmith scan.
///
/// This function prints the banner, version, or other relevant info to
/// the standard output. Can be called at the start of main().
void ropsmith_print_info(void);

#ifdef __cplusplus
}
#endif

#endif // ROPSMITH_H
