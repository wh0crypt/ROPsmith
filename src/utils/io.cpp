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
#include <stdexcept>

namespace utils::io {

/// \brief Closes a FILE pointer.
///
/// \param fp Pointer to the FILE to close.
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

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("ifstream error: could not open file");
    }

    try {
        file.read(reinterpret_cast<char *>(buf.data()), size);
    } catch (const std::ifstream::failure &e) {
        throw std::runtime_error(std::string("ifstream error: ") + e.what());
    }
}

void write_buffer_to_file(const std::vector<std::byte> &buf,
                          const std::filesystem::path  &path) {
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
}

std::FILE *open_binary(const std::filesystem::path &path) {
#ifdef _WIN32
    return _wfopen(path.c_str(), L"rb");
#else
    return std::fopen(path.c_str(), "rb");
#endif
}

} // namespace utils::io
