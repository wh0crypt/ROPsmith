//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file tests/test_finder.c
/// This file contains unit tests for the ROPsmith project.
///
//===----------------------------------------------------------------------===//

#include "../src/rop/finder.h"

#include <criterion/criterion.h>
#include <stdio.h>

#define MESON_SOURCE_ROOT getenv("MESON_SOURCE_ROOT")
#define TEST_SAMPLES_DIR "test_samples"

/// Tests for scan_elf_text_for_rets function

Test(ROPsmith, detects_rets) {
    char path[512];
    snprintf(path,
             sizeof(path),
             "%s/%s/sample_valid",
             MESON_SOURCE_ROOT,
             TEST_SAMPLES_DIR);
    int result = scan_elf_text_for_rets(path, 16);
    cr_assert(result > 0, "Expected positive number of rets, got %d", result);
}

Test(ROPsmith, handles_empty_file) {
    char path[512];
    snprintf(path,
             sizeof(path),
             "%s/%s/sample_empty",
             MESON_SOURCE_ROOT,
             TEST_SAMPLES_DIR);
    int result = scan_elf_text_for_rets(path, 16);
    cr_assert_eq(result, -1, "Expected -1 on empty file, got %d", result);
}

Test(ROPsmith, handles_non_elf_file) {
    char path[512];
    snprintf(path,
             sizeof(path),
             "%s/%s/sample_no_elf",
             MESON_SOURCE_ROOT,
             TEST_SAMPLES_DIR);
    int result = scan_elf_text_for_rets(path, 16);
    cr_assert_eq(result, -1, "Expected -1 on non-ELF file, got %d", result);
}

Test(ROPsmith, handles_no_text_section) {
    char path[512];
    snprintf(path,
             sizeof(path),
             "%s/%s/sample_no_text",
             MESON_SOURCE_ROOT,
             TEST_SAMPLES_DIR);
    int result = scan_elf_text_for_rets(path, 16);
    cr_assert_eq(
        result, -1, "Expected -1 on ELF with no .text section, got %d", result);
}
