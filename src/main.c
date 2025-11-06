//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file main.c
/// Entry point for the ROPsmith CLI tool.
///
//===----------------------------------------------------------------------===//

#include "include/ropsmith/ropsmith.h"
#include "rop/finder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// \brief Print usage information for the ROPsmith CLI tool.
///
/// \param progname Name of the program (argv[0]).
static void print_usage(const char* progname) {
    printf("ROPsmith - simple scanner MVP\n");
    printf("Usage: %s <binary> [--context N]\n", progname);
    printf("\nExamples:\n");
    printf("  %s tests/test_samples/sample_valid\n", progname);
    printf("  %s /path/to/binary --context 12\n", progname);
}

/// \brief Main entry point for the ROPsmith CLI tool.
/// \param argc Argument count.
/// \param argv Argument vector.
/// \return Exit code.
int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char* path    = argv[1];
    int         context = 16;

    // Optional args: allow "--context N"
    if (argc >= 4 && strcmp(argv[2], "--context") == 0) {
        context = atoi(argv[3]);
        if (context <= 0) {
            fprintf(stderr, "Invalid context value, using default (16).\n");
            context = 16;
        }
    } else if (argc >= 3) {
        // Handle simple "-h" or "--help"
        if (strcmp(argv[2], "-h") == 0 || strcmp(argv[2], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }

        // Or a direct numeric argument as shorthand for context
        char* endptr = NULL;
        long  maybe  = strtol(argv[2], &endptr, 10);
        if (endptr != argv[2] && maybe >= 0) {
            context = (int)maybe;
        } else {
            fprintf(stderr, "Unknown option: %s\n\n", argv[2]);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    ropsmith_print_info();

    printf("Scanning %s (context=%d)\n", path, context);
    int result = scan_elf_text_for_rets(path, context);
    if (result < 0) {
        fprintf(stderr, "Scan failed with code %d\n", result);
        return EXIT_FAILURE;
    }

    printf("Found %d RET instructions.\n", result);
    return EXIT_SUCCESS;
}
