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

#include "core/binary.h"
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
    struct program::ProgramOptions options;
    program::parse_arguments(argc, argv, options);

    if (options.show_help) {
        program::print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    if (argc < 2) {
        program::print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    ropsmith::print_info();
    std::printf("Scanning %s (context=%d):\n",
                options.binary_path.string().c_str(),
                options.context_bytes);

    std::size_t result = 0;

    try {
        file::Binary binary(options.binary_path);
        file::print_binary_info(binary);
        result = ropsmith::scan_file(binary, options.context_bytes);
    } catch (const std::runtime_error &e) {
        std::fprintf(stderr, "%s\n", e.what());
        return EXIT_FAILURE;
    }

    std::printf("Found %zu RET instructions.\n", result);
    return EXIT_SUCCESS;
}
