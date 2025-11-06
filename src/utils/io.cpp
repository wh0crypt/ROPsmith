//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file utils/io.cpp
/// Implementation of utility functions for file I/O, printing, and general
/// helpers. These functions are used internally by ROPsmith scanning routines.
///
//===----------------------------------------------------------------------===//

#define BYTES_PER_LINE 16

#include "io.h"

#ifdef __cplusplus
namespace utils::io {
extern "C" {
#endif // __cplusplus

long get_file_size(FILE *file) {
    long filesize = -1;
    fseek(file, 0, SEEK_END);
    filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    return filesize;
}

void print_bytes_hex(const unsigned char *buf, size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        printf("%02X ", buf[i]);
        if ((i + 1) % BYTES_PER_LINE == 0) {
            printf("\n");
        }
    }
}

#ifdef __cplusplus
} // extern "C"
} // namespace utils::io
#endif // __cplusplus
