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

#include "core/ropsmith.h"
#include "program.h"

#include <cstdio>
#include <cstdlib>

/// \brief Main entry point for the ROPsmith CLI tool.
///
/// \param argc Argument count.
/// \param argv Argument vector.
/// \return Exit code.
int main(int argc, char **argv) {
    struct program::program_options options;
    program::parse_arguments(argc, argv, options);

    if (options.show_help) {
        program::print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    if (argc < 2) {
        program::print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (options.error_code != EXIT_SUCCESS) {
        std::fprintf(stderr, "Error: %s\n", options.error_msg.c_str());
        return options.error_code;
    }

    ropsmith::print_info();

    std::printf("Scanning %s (context=%d):\n",
                options.binary_path.c_str(),
                options.context_bytes);
    int result =
        ropsmith::scan_file(options.binary_path.c_str(), options.context_bytes);
    if (result < 0) {
        std::fprintf(stderr, "Error: Scan failed with code %d\n", result);
        return EXIT_FAILURE;
    }

    std::printf("Found %d RET instructions.\n", result);
    return EXIT_SUCCESS;
}
