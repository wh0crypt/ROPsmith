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

#include "io.h"

#include <cstdio>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <vector>
#include <zlib.h>

namespace utils::io {

void FileCloser::operator()(std::FILE *fp) const noexcept {
    if (fp) {
        std::fclose(fp);
    }
}

void read_file_to_buffer(const std::filesystem::path &path,
                         std::vector<std::byte>      &buf) {
    std::uintmax_t size = 0;

    try {
        size = std::filesystem::file_size(path);
    } catch (const std::filesystem::filesystem_error &e) {
        throw std::runtime_error(std::string("filesystem_error: ") + e.what());
    }

    if (size == 0) {
        throw std::runtime_error("file is empty");
    }

    buf.resize(size);

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("ifstream error: could not open file");
    }

    try {
        file.read(reinterpret_cast<char *>(buf.data()),
                  static_cast<std::streamsize>(size));
    } catch (const std::ifstream::failure &e) {
        throw std::runtime_error(std::string("ifstream error: ") + e.what());
    }

    if (!file || file.gcount() != static_cast<std::streamsize>(size)) {
        throw std::runtime_error("ifstream error: incomplete read");
    }
}

void write_buffer_to_file(const std::vector<std::byte> &buf,
                          const std::filesystem::path  &path,
                          std::uint32_t                 expected_crc) {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("ofstream error: could not open file");
    }

    try {
        file.write(reinterpret_cast<const char *>(buf.data()),
                   static_cast<std::streamsize>(buf.size()));
    } catch (const std::ofstream::failure &e) {
        throw std::runtime_error(std::string("ofstream error: ") + e.what());
    }

    if (!file) {
        throw std::runtime_error("ofstream error: incomplete write");
    }

    std::uintmax_t written_size = 0;
    try {
        written_size = std::filesystem::file_size(path);
    } catch (const std::filesystem::filesystem_error &e) {
        throw std::runtime_error(std::string("filesystem_error after write: ") +
                                 e.what());
    }

    if (written_size != buf.size()) {
        throw std::runtime_error(
            "ofstream error: incomplete write (size mismatch)");
    }

    if (expected_crc != 0) {
        std::vector<std::byte> verify_buf;
        read_file_to_buffer(path, verify_buf);

        std::uint32_t actual_crc = compute_crc32(verify_buf);
        if (actual_crc != expected_crc) {
            throw std::runtime_error(
                "ofstream error: CRC32 mismatch after write (data corrupted)");
        }
    }
}

std::uint32_t compute_crc32(const std::vector<std::byte> &buf) {
    return crc32(0L, reinterpret_cast<const Bytef *>(buf.data()), buf.size());
}

std::FILE *open_binary(const std::filesystem::path &path) {
#ifdef _WIN32
    return _wfopen(path.c_str(), L"rb");
#else
    return std::fopen(path.c_str(), "rb");
#endif
}

} // namespace utils::io
