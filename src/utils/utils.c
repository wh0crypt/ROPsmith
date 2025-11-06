//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file utils/utils.c
/// This file contains implementations and helper functions for the ROPsmith
/// project, including ELF scanning, ROP gadget detection, and utility
/// functions used across the project.
///
//===----------------------------------------------------------------------===//

#define BYTES_PER_LINE 16

#include "utils.h"

long get_file_size(FILE* file) {
    long filesize = -1;
    fseek(file, 0, SEEK_END);
    filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    return filesize;
}

void print_bytes_hex(const unsigned char* buf, size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        printf("%02X ", buf[i]);
        if ((i + 1) % BYTES_PER_LINE == 0) {
            printf("\n");
        }
    }
}
