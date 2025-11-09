//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file program.cpp
/// Implementation of functions for command-line argument handling.
///
//===----------------------------------------------------------------------===//

#include "program.h"

#include "macros.h"

#include <stdexcept>
#include <string_view>
#include <vector>

namespace program {

void parse_arguments(int argc, char **argv, ProgramOptions &options) {
    std::vector<std::string_view> args(argv + 1, argv + argc);

    for (auto it = args.begin(); it != args.end(); ++it) {
        if (*it == "--help" || *it == "-h") {
            options.show_help = true;
            return;
        }

        if (*it == "--context" || *it == "-c") {
            if (++it != args.end()) {
                try {
                    options.context_bytes = std::stoi(std::string(*it));
                    continue;
                } catch (const std::invalid_argument &) {
                    options.error_msg  = "Invalid context value.";
                    options.error_code = 1;
                    return;
                }
            }

            options.error_msg  = "--context requires a value.";
            options.error_code = 1;
            return;
        }

        if (options.binary_path.empty()) {
            options.binary_path = std::filesystem::path(*it);
        } else {
            options.error_msg  = "Unknown argument: " + std::string(*it);
            options.error_code = 1;
            return;
        }
    }

    if (!options.show_help && options.binary_path.empty()) {
        options.error_msg  = "No binary path provided.";
        options.error_code = 1;
    }
}

void print_usage(const std::string_view progname) {
    std::printf("Usage: %s [-h | --help] <binary> [--context N | -c N]\n",
                progname.data());
}

void print_help(const std::string_view progname) {
    print_usage(progname);
    std::printf(
        "\nOptions:\n"
        "-h, --help\t\tShow this help message and exit.\n"
        "-c N, --context N\tSet the number of context bytes (default: %d).\n",
        DEFAULT_CONTEXT_BYTES);
}

} // namespace program
