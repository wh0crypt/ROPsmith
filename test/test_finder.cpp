//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file tests/test_finder.c
/// Unit tests for the ROPsmith project.
///
//===----------------------------------------------------------------------===//

#define TEST_SAMPLES_DIR getenv("TEST_SAMPLES_DIR")

#include "../src/core/rop/finder.h"
#include "../src/macros.h"

#include <criterion/criterion.h>
#include <string>

Test(ROPsmith, detects_rets) {
    std::string path_str = std::string(TEST_SAMPLES_DIR) + "/sample_valid";
    int result = find_ret_instructions(path_str.c_str(), DEFAULT_CONTEXT_BYTES);
    cr_assert(result > 0, "Expected positive number of rets, got %d", result);
}

Test(ROPsmith, handles_empty_file) {
    std::string path_str = std::string(TEST_SAMPLES_DIR) + "/sample_empty";
    int result = find_ret_instructions(path_str.c_str(), DEFAULT_CONTEXT_BYTES);
    cr_assert_eq(result, -1, "Expected -1 on empty file, got %d", result);
}

Test(ROPsmith, handles_non_elf_file) {
    std::string path_str = std::string(TEST_SAMPLES_DIR) + "/sample_no_elf";
    int result = find_ret_instructions(path_str.c_str(), DEFAULT_CONTEXT_BYTES);
    cr_assert_eq(result, -1, "Expected -1 on non-ELF file, got %d", result);
}

Test(ROPsmith, handles_no_text_section) {
    std::string path_str = std::string(TEST_SAMPLES_DIR) + "/sample_no_text";
    int result = find_ret_instructions(path_str.c_str(), DEFAULT_CONTEXT_BYTES);
    cr_assert_eq(
        result, -1, "Expected -1 on ELF with no .text section, got %d", result);
}
