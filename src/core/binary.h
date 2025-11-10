//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file core/binary.h
/// Definitions and utilities for handling binary files. Creates a class that
/// serves as an interface for different binary formats.
///
//===----------------------------------------------------------------------===//

#ifndef CORE_BINARY_H
#define CORE_BINARY_H

#include <cstddef>
#include <cstring>
#include <filesystem>
#include <vector>

namespace file {

typedef enum BinaryType { UnknownType = 0, ELF, PE, MachO } BinaryType;

typedef enum Bitness { UnknownBitness = 0, x32, x64 } Bitness;

typedef enum Endianness {
    UnknownEndian = 0,
    LittleEndian,
    BigEndian
} Endianness;

typedef enum Architecture {
    UnknownArch = 0,
    x86,
    AMD64,
    ARM,
    AArch64,
    RISCV,
    MIPS
} Architecture;

std::string binary_type_to_string(const BinaryType &type);
std::string bitness_to_string(const Bitness &bitness);
std::string endianness_to_string(const Endianness &endianness);
std::string arch_to_string(const Architecture &arch);

class Binary {
  public:
    Binary();
    Binary(const std::filesystem::path &path);

    /// \brief Load a binary file from the specified path.
    ///
    /// \param path The path to the binary file.
    /// \throws std::runtime_error if the file cannot be loaded.
    void load(const std::filesystem::path &path);

    /// \brief Save the binary file to the specified path.
    ///
    /// \param path The path to the binary file.
    /// \throws std::runtime_error if the file cannot be saved.
    void save(const std::filesystem::path &path) const;

    /// \brief Get the path to the binary file.
    ///
    /// \return The path to the binary file.
    const std::filesystem::path &path() const;

    /// \brief Get the raw binary data.
    ///
    /// \return A pointer to the raw binary data.
    const std::byte *data() const;

    /// \brief Get the size of the binary data.
    ///
    /// \return The size of the binary data in bytes.
    std::size_t size() const;

    /// \brief Get the binary data as a vector of bytes.
    ///
    /// \return A reference to the vector containing the binary data.
    const std::vector<std::byte> &vector() const;

    /// \brief Get the type of the binary file.
    ///
    /// \return The type of the binary file.
    const BinaryType &type() const;

    /// \brief Get the bitness of the binary file.
    ///
    /// \return The bitness of the binary file.
    const Bitness &bitness() const;

    /// \brief Get the endianness of the binary file.
    ///
    /// \return The endianness of the binary file.
    const Endianness &endianness() const;

    /// \brief Get the architecture of the binary file.
    ///
    /// \return The architecture of the binary file.
    const Architecture &architecture() const;

    /// \brief Access a byte at the specified index.
    ///
    /// \param index The index of the byte to access.
    /// \return A reference to the byte at the specified index.
    const std::byte &operator[](std::size_t index) const;

  private:
    std::filesystem::path  path_;
    std::vector<std::byte> data_;
    BinaryType             type_;
    Bitness                bitness_;
    Endianness             endianness_;
    Architecture           architecture_;

    /// \brief Find the type of the binary file.
    void find_type();

    /// \brief Find the bitness of the binary file.
    void find_bitness();

    /// \brief Find the endianness of the binary file.
    void find_endianness();

    /// \brief Find the architecture of the binary file.
    void find_architecture();

    /// \brief Read bytes from the binary data into a variable.
    ///
    /// \tparam T The type of the variable to read into.
    /// \param buf The buffer containing the binary data.
    /// \param offset The offset in the buffer to start reading from.
    /// \param out The variable to read the data into.
    /// \return True if the read was successful, false otherwise.
    template <typename T>
    bool read_bytes(const std::vector<std::byte> &buf, size_t offset, T &out);
};

/// \brief Print information about the binary file.
///
/// \param binary The binary file to print information about.
void print_binary_info(const Binary &binary);

template <typename T>
bool Binary::read_bytes(const std::vector<std::byte> &buf,
                        size_t                        offset,
                        T                            &out) {
    if (offset + sizeof(T) > buf.size()) {
        return false;
    }

    std::memcpy(&out, buf.data() + offset, sizeof(T));
    return true;
}

} // namespace file

#endif // CORE_BINARY_H
