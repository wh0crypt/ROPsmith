//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file utils/utils.h
/// Utility functions for file I/O, printing, and general helpers.
/// These functions are used internally by ROPsmith scanning routines.
///
//===----------------------------------------------------------------------===//

#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/// \brief Get the size of the given file in bytes.
///
/// \param file Pointer to the opened file.
/// \return Returns the file size in bytes, or -1 on error.
long get_file_size(FILE* file);

/// \brief Reads the entire contents of a file into a buffer.
///
/// Allocates memory for the file contents, which the caller must free.
/// On success, sets \p size to the file size and returns the buffer.
/// On failure, returns NULL.
///
/// \param path Path to the file to read.
/// \param size Pointer to size variable to store the number of bytes read.
/// \return Pointer to allocated buffer containing file contents, or NULL on error.
unsigned char* read_file_to_buffer(const char* path, size_t* size);

/// \brief Prints a buffer in hexadecimal format.
///
/// \param buf Pointer to the buffer to print.
/// \param start Starting index in the buffer to print.
/// \param end Ending index in the buffer to print.
void print_bytes_hex(const unsigned char* buf, size_t start, size_t end);

#ifdef __cplusplus
}
#endif

#endif // UTILS_UTILS_H
