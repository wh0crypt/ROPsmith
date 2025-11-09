//===----------------------------------------------------------------------===//
///
/// \file core/binary.h
/// Definitions and utilities for handling binary files.
///
//===----------------------------------------------------------------------===//

#include "binary.h"

#include "utils/io.h"

#include <cstring>
#include <stdexcept>
#include <sys/types.h>

#if defined(__APPLE__) || defined(_WIN32)
#include "include/elf.h"
#else
#include <elf.h>
#endif // ELF headers

#if defined(_WIN32)
#include <windows.h>
#include <winnt.h>
#else
#include "include/winnt_min.h"
#endif // Windows headers

namespace file {

Binary::Binary() {
    this->path_ = std::filesystem::path();
    this->data_.clear();
    this->type_         = BinaryType::UnknownType;
    this->endianness_   = Endianness::UnknownEndian;
    this->architecture_ = Architecture::UnknownArch;
}

Binary::Binary(const std::filesystem::path &path) : path_(path) {
    this->load(path);
    this->type_         = this->find_type();
    this->endianness_   = this->find_endianness();
    this->architecture_ = this->find_architecture();
}

void Binary::load(const std::filesystem::path &path) {
    try {
        utils::io::read_file_to_buffer(path, this->data_);
    } catch (const std::runtime_error &e) {
        throw std::runtime_error(std::string("Error loading binary file: ") +
                                 e.what());
    }
}

void Binary::save(const std::filesystem::path &path) const {
    try {
        utils::io::write_buffer_to_file(this->data_, path);
    } catch (const std::runtime_error &e) {
        throw std::runtime_error(std::string("Error saving binary file: ") +
                                 e.what());
    }
}

const std::filesystem::path &Binary::path() const { return this->path_; }
const std::byte             *Binary::data() const { return this->data_.data(); }
std::size_t                  Binary::size() const { return this->data_.size(); }
const std::vector<std::byte> &Binary::vector() const { return this->data_; }
const BinaryType             &Binary::type() const { return this->type_; }
const Endianness   &Binary::endianness() const { return this->endianness_; }
const Architecture &Binary::architecture() const { return this->architecture_; }

BinaryType Binary::find_type() {
    if (this->size() < 4) {
        return BinaryType::UnknownType;
    }

    // ELF: 0x7F 'E' 'L' 'F'
    // ELF magic: 0x7F 'E' 'L' 'F'
    if (this->vector()[0] == std::byte{0x7F} &&
        this->vector()[1] == std::byte{'E'} &&
        this->vector()[2] == std::byte{'L'} &&
        this->vector()[3] == std::byte{'F'}) {
        return BinaryType::ELF;
    }

    // PE: bytes 0..1 = 'M' 'Z' (0x4D 0x5A)
    if (this->vector()[0] == std::byte{'M'} &&
        this->vector()[1] == std::byte{'Z'}) {
        return BinaryType::PE;
    }

    // Mach-O: bytes 0..3 = magic
    // 0xFEEDFACE  -> 32-bit little endian
    // 0xCEFAEDFE  -> 32-bit big endian
    // 0xFEEDFACF  -> 64-bit little endian
    // 0xCFFAEDFE  -> 64-bit big endian
    std::uint32_t magic = 0;
    if (!read_bytes(this->vector(), 0, magic)) {
        return BinaryType::UnknownType;
    }

    if (magic == 0 || magic == 0xFFFFFFFF) {
        return BinaryType::UnknownType;
    }

    switch (magic) {
        case 0xFEEDFACE:
        case 0xFEEDFACF:
        case 0xCEFAEDFE:
        case 0xCFFAEDFE:
            return BinaryType::MachO;
    }

    return BinaryType::UnknownType;
}

Endianness Binary::find_endianness() {
    if (this->size() < 8) {
        return Endianness::UnknownEndian;
    }

    if (this->type() == BinaryType::ELF) {
        std::byte ei_data = this->vector()[5];

        if (ei_data == std::byte{1}) {
            return Endianness::LittleEndian;
        }

        if (ei_data == std::byte{2}) {
            return Endianness::BigEndian;
        }

        return Endianness::UnknownEndian;
    }

    if (this->type() == BinaryType::PE) {
        return Endianness::LittleEndian;
    }

    if (this->type() == BinaryType::MachO) {
        // Mach-O: bytes 0..3 = magic
        // 0xFEEDFACE  -> 32-bit little endian
        // 0xCEFAEDFE  -> 32-bit big endian
        // 0xFEEDFACF  -> 64-bit little endian
        // 0xCFFAEDFE  -> 64-bit big endian
        std::uint32_t magic = 0;
        if (!read_bytes(this->vector(), 0, magic)) {
            return Endianness::UnknownEndian;
        }

        if (magic == 0 || magic == 0xFFFFFFFF) {
            return Endianness::UnknownEndian;
        }

        switch (magic) {
            case 0xFEEDFACE:
            case 0xFEEDFACF:
                return Endianness::LittleEndian;
            case 0xCEFAEDFE:
            case 0xCFFAEDFE:
                return Endianness::BigEndian;
            default:
                return Endianness::UnknownEndian;
        }
    }

    return Endianness::UnknownEndian;
}

Architecture Binary::find_architecture() {
    if (this->size() < 4 || this->type() == BinaryType::UnknownType) {
        return Architecture::UnknownArch;
    }

    if (this->type() == BinaryType::ELF) {
        if (this->size() < 0x14) {
            return Architecture::UnknownArch;
        }

        // e_machine is at offset 0x12 (2 bytes) in ELF header
        uint16_t e_machine = 0;
        if (!read_bytes(this->vector(), 0x12, e_machine)) {
            return Architecture::UnknownArch;
        }

        // Endianness handling
        if (this->endianness() == Endianness::BigEndian) {
            e_machine = utils::io::swap_bytes(e_machine);
        }

        switch (e_machine) {
            case EM_386:
                return Architecture::x86;
            case EM_X86_64:
                return Architecture::AMD64;
            case EM_ARM:
                return Architecture::ARM;
            case EM_AARCH64:
                return Architecture::AArch64;
            case EM_RISCV:
                return Architecture::RISCV;
            case EM_MIPS:
                return Architecture::MIPS;
            default:
                return Architecture::UnknownArch;
        }
    }

    if (this->type() == BinaryType::PE) {
        if (this->size() < 0x40) {
            return Architecture::UnknownArch;
        }

        // PE header offset is at 0x3C (4 bytes)
        std::uint32_t pe_offset = 0;
        if (!read_bytes(this->vector(), 0x3C, pe_offset)) {
            return Architecture::UnknownArch;
        }

        // 4 bytes (offset) + 2 bytes (machine)
        if (pe_offset > this->size() - 6) {
            return Architecture::UnknownArch;
        }

        // Machine field is at offset pe_offset + 4 (2 bytes)
        uint16_t machine = 0;
        if (!read_bytes(this->vector(), pe_offset + 4, machine)) {
            return Architecture::UnknownArch;
        }

        switch (machine) {
            case IMAGE_FILE_MACHINE_UNKNOWN:
                return Architecture::UnknownArch;
            case IMAGE_FILE_MACHINE_I386:
                return Architecture::x86;
            case IMAGE_FILE_MACHINE_AMD64:
                return Architecture::AMD64;
            case IMAGE_FILE_MACHINE_ARM:
            case IMAGE_FILE_MACHINE_THUMB:
            case IMAGE_FILE_MACHINE_ARMNT:
                return Architecture::ARM;
            case IMAGE_FILE_MACHINE_ARM64:
                return Architecture::AArch64;
            case IMAGE_FILE_MACHINE_MIPS16:
            case IMAGE_FILE_MACHINE_MIPSFPU:
            case IMAGE_FILE_MACHINE_MIPSFPU16:
                return Architecture::MIPS;
            default:
                return Architecture::UnknownArch;
        }
    }

    if (this->type() == BinaryType::MachO) {
        // magic (4) + cputype (4) + cpusubtype (4)
        if (this->size() < 12) {
            return Architecture::UnknownArch;
        }

        std::uint32_t magic = 0;
        if (!read_bytes(this->vector(), 0, magic)) {
            return Architecture::UnknownArch;
        }

        // Fat Mach-O
        if (magic == 0xCAFEBABE || magic == 0xBEBAFECA || magic == 0xCAFED00D ||
            magic == 0xD00DFECA) {
            bool is_big_endian = (magic == 0xCAFEBABE || magic == 0xCAFED00D);
            std::uint32_t nfat_arch = 0;

            if (!read_bytes(this->vector(), 4, nfat_arch)) {
                return Architecture::UnknownArch;
            }

            if (!is_big_endian) {
            }

            // fat_arch structure is 20 bytes
            std::size_t arch_offset = 8;
            for (std::uint32_t i = 0; i < nfat_arch; ++i) {
                if (arch_offset + 8 > this->size()) {
                    break;
                }

                std::uint32_t cputype = 0;
                if (!read_bytes(this->vector(), arch_offset, cputype)) {
                    break;
                }

                if (!is_big_endian) {
                    cputype = utils::io::swap_bytes(cputype);
                }

                switch (cputype) {
                    case 7: // CPU_TYPE_X86
                        return Architecture::x86;
                    case 0x01000007: // CPU_TYPE_X86_64
                        return Architecture::AMD64;
                    case 12: // CPU_TYPE_ARM
                        return Architecture::ARM;
                    case 0x0100000C: // CPU_TYPE_ARM64
                        return Architecture::AArch64;
                    default:
                        break;
                }

                arch_offset += 20;
            }

            return Architecture::UnknownArch;
        }

        // Normal Mach-O
        std::uint32_t cputype = 0;
        if (!read_bytes(this->vector(), 4, cputype)) {
            return Architecture::UnknownArch;
        }

        if (magic == 0 || magic == 0xFFFFFFFF) {
            return Architecture::UnknownArch;
        }

        if (this->endianness() == Endianness::BigEndian) {
            cputype = utils::io::swap_bytes(cputype);
        }

        switch (cputype) {
            case 7: // CPU_TYPE_X86
                return Architecture::x86;
            case 0x01000007: // CPU_TYPE_X86_64
                return Architecture::AMD64;
            case 12: // CPU_TYPE_ARM
                return Architecture::ARM;
            case 0x0100000C: // CPU_TYPE_ARM64
                return Architecture::AArch64;
            default:
                return Architecture::UnknownArch;
        }
    }

    return Architecture::UnknownArch;
}

const std::byte &Binary::operator[](std::size_t index) const {
    return this->data_.at(index);
}

} // namespace file
