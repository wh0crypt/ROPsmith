//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file program.h
/// Functions for command-line argument handling.
///
//===----------------------------------------------------------------------===//

#ifndef PROGRAM_H
#define PROGRAM_H

#include "macros.h"

#include <string>

namespace program {

/// \brief Structure to hold parsed program options.
///
/// This structure contains the binary path, context bytes, error messages,
/// error codes, and a flag to indicate if help should be shown.
struct program_options {
    std::string binary_path   = "";
    int         context_bytes = DEFAULT_CONTEXT_BYTES;
    std::string error_msg     = "";
    int         error_code    = EXIT_SUCCESS;
    bool        show_help     = false;
};

/// \brief Parse command-line arguments.
///
/// This function processes the command-line arguments and populates
/// a `program_options` structure with the parsed values.
///
/// \param argc Argument count.
/// \param argv Argument vector.
/// \param options Reference to a `program_options` structure to populate.
void parse_arguments(int argc, char **argv, program_options &options);

/// \brief Print usage information for the ROPsmith CLI tool.
///
/// \param progname Name of the program (argv[0]).
void print_usage(const std::string_view progname);

/// \brief Print detailed help information for the ROPsmith CLI tool.
///
/// \param progname Name of the program (argv[0]).
void print_help(const std::string_view progname);

} // namespace program

#endif // PROGRAM_H
