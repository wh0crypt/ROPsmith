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

#include "core/rop/scanner.h"

#include <criterion/criterion.h>
#include <filesystem>

const std::string TEST_SAMPLES_DIR = std::getenv("TEST_SAMPLES_DIR");

Test(ROPsmith, detects_rets) {
    std::filesystem::path path =
        std::filesystem::path(TEST_SAMPLES_DIR) / "sample_valid";
    int result = rop::find_ret_instructions(path);
    cr_expect(result > 0, "Expected positive number of rets, got %d", result);
}

Test(ROPsmith, handles_empty_file) {
    std::filesystem::path path =
        std::filesystem::path(TEST_SAMPLES_DIR) / "sample_empty";
    cr_expect_throw(rop::find_ret_instructions(path),
                    std::runtime_error,
                    "Expected exception on empty file");
}

Test(ROPsmith, handles_non_elf_file) {
    std::filesystem::path path =
        std::filesystem::path(TEST_SAMPLES_DIR) / "sample_no_elf";
    cr_expect_throw(rop::find_ret_instructions(path),
                    std::runtime_error,
                    "Expected exception on non-ELF file");
}

Test(ROPsmith, handles_no_text_section) {
    std::filesystem::path path =
        std::filesystem::path(TEST_SAMPLES_DIR) / "sample_no_text";
    cr_expect_throw(rop::find_ret_instructions(path),
                    std::runtime_error,
                    "Expected exception on ELF with no .text section");
}
