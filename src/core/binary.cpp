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

std::string binary_type_to_string(const BinaryType &type) {
    switch (type) {
        case BinaryType::UnknownType:
            return "N/A";
        case BinaryType::ELF:
            return "ELF";
        case BinaryType::PE:
            return "PE";
        case BinaryType::MachO:
            return "Mach-O";
    }

    return "N/A";
}

std::string bitness_to_string(const Bitness &bitness) {
    switch (bitness) {
        case Bitness::UnknownBitness:
            return "N/A";
        case Bitness::x32:
            return "32-bit";
        case Bitness::x64:
            return "64-bit";
    }

    return "N/A";
}

std::string endianness_to_string(const Endianness &endianness) {
    switch (endianness) {
        case Endianness::UnknownEndian:
            return "N/A";
        case Endianness::LittleEndian:
            return "LSB";
        case Endianness::BigEndian:
            return "MSB";
    }

    return "N/A";
}

std::string arch_to_string(const Architecture &arch) {
    switch (arch) {
        case Architecture::UnknownArch:
            return "N/A";
        case Architecture::x86:
            return "x86";
        case Architecture::AMD64:
            return "x86_64";
        case Architecture::ARM:
            return "arm";
        case Architecture::AArch64:
            return "aarch64";
        case Architecture::RISCV:
            return "riscv";
        case Architecture::MIPS:
            return "mips";
    }

    return "N/A";
}

Binary::Binary() {
    this->path_ = std::filesystem::path();
    this->data_.clear();
    this->type_         = BinaryType::UnknownType;
    this->bitness_      = Bitness::UnknownBitness;
    this->endianness_   = Endianness::UnknownEndian;
    this->architecture_ = Architecture::UnknownArch;
}

Binary::Binary(const std::filesystem::path &path) : path_(path) {
    try {
        this->load(path);
        this->find_type();
        this->find_bitness();
        this->find_endianness();
        this->find_architecture();
    } catch (const std::runtime_error &e) {
        throw e;
    }
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
const Bitness                &Binary::bitness() const { return this->bitness_; }
const Endianness   &Binary::endianness() const { return this->endianness_; }
const Architecture &Binary::architecture() const { return this->architecture_; }

void Binary::find_type() {
    if (this->size() < 4) {
        this->type_ = BinaryType::UnknownType;
        return;
    }

    // ELF: 0x7F 'E' 'L' 'F'
    // ELF magic: 0x7F 'E' 'L' 'F'
    if (this->vector()[0] == std::byte{0x7F} &&
        this->vector()[1] == std::byte{'E'} &&
        this->vector()[2] == std::byte{'L'} &&
        this->vector()[3] == std::byte{'F'}) {
        this->type_ = BinaryType::ELF;
        return;
    }

    // PE: bytes 0..1 = 'M' 'Z' (0x4D 0x5A)
    if (this->vector()[0] == std::byte{'M'} &&
        this->vector()[1] == std::byte{'Z'}) {
        this->type_ = BinaryType::PE;
        return;
    }

    // Mach-O: bytes 0..3 = magic
    // 0xFEEDFACE  -> 32-bit little endian
    // 0xCEFAEDFE  -> 32-bit big endian
    // 0xFEEDFACF  -> 64-bit little endian
    // 0xCFFAEDFE  -> 64-bit big endian
    std::uint32_t magic = 0;
    if (!read_bytes(this->vector(), 0, magic)) {
        this->type_ = BinaryType::UnknownType;
        return;
    }

    if (magic == 0 || magic == 0xFFFFFFFF) {
        this->type_ = BinaryType::UnknownType;
        return;
    }

    switch (magic) {
        case 0xFEEDFACE:
        case 0xFEEDFACF:
        case 0xCEFAEDFE:
        case 0xCFFAEDFE:
            this->type_ = BinaryType::MachO;
            return;
    }

    this->type_ = BinaryType::UnknownType;
}

void Binary::find_bitness() {
    if (this->type() == BinaryType::ELF) {
        std::byte ei_class = this->vector()[4];

        if (this->size() > 5) {
            if (ei_class == std::byte{1}) {
                this->bitness_ = Bitness::x32;
                return;
            }

            if (ei_class == std::byte{2}) {
                this->bitness_ = Bitness::x64;
                return;
            }
        }

        this->bitness_ = Bitness::UnknownBitness;
        return;
    }

    if (this->type() == BinaryType::PE) {
        if (this->size() < 0x40) {
            this->bitness_ = Bitness::UnknownBitness;
            return;
        }

        // PE header offset is at 0x3C (4 bytes)
        std::uint32_t pe_offset = 0;
        if (!read_bytes(this->vector(), 0x3C, pe_offset)) {
            this->bitness_ = Bitness::UnknownBitness;
            return;
        }

        // 4 bytes (offset) + 2 bytes (machine)
        if (pe_offset > this->size() - 6) {
            this->bitness_ = Bitness::UnknownBitness;
            return;
        }

        // Magic field is at offset pe_offset + 4 (2 bytes)
        uint16_t magic = 0;
        if (!read_bytes(this->vector(), pe_offset + 0x18, magic)) {
            this->bitness_ = Bitness::UnknownBitness;
            return;
        }

        switch (magic) {
            case 0x10b:
                this->bitness_ = Bitness::x32;
                return;
            case 0x20b:
                this->bitness_ = Bitness::x64;
                return;
            default:
                this->bitness_ = Bitness::UnknownBitness;
                return;
        }
    }

    if (this->type() == BinaryType::MachO) {
        uint32_t magic = 0;
        if (!read_bytes(this->vector(), 0, magic)) {
            this->bitness_ = Bitness::UnknownBitness;
            return;
        }

        switch (magic) {
            case 0xFEEDFACE:
            case 0xCEFAEDFE:
                this->bitness_ = Bitness::x32;
                return;
            case 0xFEEDFACF:
            case 0xCFFAEDFE:
                this->bitness_ = Bitness::x64;
                return;
            default:
                this->bitness_ = Bitness::UnknownBitness;
                return;
        }
    }
}

void Binary::find_endianness() {
    if (this->size() < 8) {
        this->endianness_ = Endianness::UnknownEndian;
        return;
    }

    if (this->type() == BinaryType::ELF) {
        std::byte ei_data = this->vector()[5];

        if (ei_data == std::byte{1}) {
            this->endianness_ = Endianness::LittleEndian;
            return;
        }

        if (ei_data == std::byte{2}) {
            this->endianness_ = Endianness::BigEndian;
            return;
        }

        this->endianness_ = Endianness::UnknownEndian;
        return;
    }

    if (this->type() == BinaryType::PE) {
        this->endianness_ = Endianness::LittleEndian;
        return;
    }

    if (this->type() == BinaryType::MachO) {
        // Mach-O: bytes 0..3 = magic
        // 0xFEEDFACE  -> 32-bit little endian
        // 0xCEFAEDFE  -> 32-bit big endian
        // 0xFEEDFACF  -> 64-bit little endian
        // 0xCFFAEDFE  -> 64-bit big endian
        std::uint32_t magic = 0;
        if (!read_bytes(this->vector(), 0, magic)) {
            this->endianness_ = Endianness::UnknownEndian;
            return;
        }

        if (magic == 0 || magic == 0xFFFFFFFF) {
            this->endianness_ = Endianness::UnknownEndian;
            return;
        }

        switch (magic) {
            case 0xFEEDFACE:
            case 0xFEEDFACF:
                this->endianness_ = Endianness::LittleEndian;
                return;
            case 0xCEFAEDFE:
            case 0xCFFAEDFE:
                this->endianness_ = Endianness::BigEndian;
                return;
            default:
                this->endianness_ = Endianness::UnknownEndian;
                return;
        }
    }

    this->endianness_ = Endianness::UnknownEndian;
}

void Binary::find_architecture() {
    if (this->size() < 4 || this->type() == BinaryType::UnknownType) {
        this->architecture_ = Architecture::UnknownArch;
        return;
    }

    if (this->type() == BinaryType::ELF) {
        if (this->size() < 0x14) {
            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        // e_machine is at offset 0x12 (2 bytes) in ELF header
        uint16_t e_machine = 0;
        if (!read_bytes(this->vector(), 0x12, e_machine)) {
            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        // Endianness handling
        if (this->endianness() == Endianness::BigEndian) {
            e_machine = utils::io::swap_bytes(e_machine);
        }

        switch (e_machine) {
            case EM_386:
                this->architecture_ = Architecture::x86;
                return;
            case EM_X86_64:
                this->architecture_ = Architecture::AMD64;
                return;
            case EM_ARM:
                this->architecture_ = Architecture::ARM;
                return;
            case EM_AARCH64:
                this->architecture_ = Architecture::AArch64;
                return;
            case EM_RISCV:
                this->architecture_ = Architecture::RISCV;
                return;
            case EM_MIPS:
                this->architecture_ = Architecture::MIPS;
                return;
            default:
                this->architecture_ = Architecture::UnknownArch;
                return;
        }
    }

    if (this->type() == BinaryType::PE) {
        if (this->size() < 0x40) {
            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        // PE header offset is at 0x3C (4 bytes)
        std::uint32_t pe_offset = 0;
        if (!read_bytes(this->vector(), 0x3C, pe_offset)) {
            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        // 4 bytes (offset) + 2 bytes (machine)
        if (pe_offset > this->size() - 6) {
            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        // Machine field is at offset pe_offset + 4 (2 bytes)
        uint16_t machine = 0;
        if (!read_bytes(this->vector(), pe_offset + 4, machine)) {
            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        switch (machine) {
            case IMAGE_FILE_MACHINE_UNKNOWN:
                this->architecture_ = Architecture::UnknownArch;
                return;
            case IMAGE_FILE_MACHINE_I386:
                this->architecture_ = Architecture::x86;
                return;
            case IMAGE_FILE_MACHINE_AMD64:
                this->architecture_ = Architecture::AMD64;
                return;
            case IMAGE_FILE_MACHINE_ARM:
            case IMAGE_FILE_MACHINE_THUMB:
            case IMAGE_FILE_MACHINE_ARMNT:
                this->architecture_ = Architecture::ARM;
                return;
            case IMAGE_FILE_MACHINE_ARM64:
                this->architecture_ = Architecture::AArch64;
                return;
            case IMAGE_FILE_MACHINE_MIPS16:
            case IMAGE_FILE_MACHINE_MIPSFPU:
            case IMAGE_FILE_MACHINE_MIPSFPU16:
                this->architecture_ = Architecture::MIPS;
                return;
            default:
                this->architecture_ = Architecture::UnknownArch;
                return;
        }
    }

    if (this->type() == BinaryType::MachO) {
        // magic (4) + cputype (4) + cpusubtype (4)
        if (this->size() < 12) {
            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        std::uint32_t magic = 0;
        if (!read_bytes(this->vector(), 0, magic)) {
            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        // Fat Mach-O
        if (magic == 0xCAFEBABE || magic == 0xBEBAFECA || magic == 0xCAFED00D ||
            magic == 0xD00DFECA) {
            bool is_big_endian = (magic == 0xCAFEBABE || magic == 0xCAFED00D);
            std::uint32_t nfat_arch = 0;

            if (!read_bytes(this->vector(), 4, nfat_arch)) {
                this->architecture_ = Architecture::UnknownArch;
                return;
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
                        this->architecture_ = Architecture::x86;
                        return;
                    case 0x01000007: // CPU_TYPE_X86_64
                        this->architecture_ = Architecture::AMD64;
                        return;
                    case 12: // CPU_TYPE_ARM
                        this->architecture_ = Architecture::ARM;
                        return;
                    case 0x0100000C: // CPU_TYPE_ARM64
                        this->architecture_ = Architecture::AArch64;
                        return;
                    default:
                        break;
                }

                arch_offset += 20;
            }

            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        // Normal Mach-O
        std::uint32_t cputype = 0;
        if (!read_bytes(this->vector(), 4, cputype)) {
            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        if (magic == 0 || magic == 0xFFFFFFFF) {
            this->architecture_ = Architecture::UnknownArch;
            return;
        }

        if (this->endianness() == Endianness::BigEndian) {
            cputype = utils::io::swap_bytes(cputype);
        }

        switch (cputype) {
            case 7: // CPU_TYPE_X86
                this->architecture_ = Architecture::x86;
                return;
            case 0x01000007: // CPU_TYPE_X86_64
                this->architecture_ = Architecture::AMD64;
                return;
            case 12: // CPU_TYPE_ARM
                this->architecture_ = Architecture::ARM;
                return;
            case 0x0100000C: // CPU_TYPE_ARM64
                this->architecture_ = Architecture::AArch64;
                return;
            default:
                this->architecture_ = Architecture::UnknownArch;
                return;
        }
    }

    this->architecture_ = Architecture::UnknownArch;
}

const std::byte &Binary::operator[](std::size_t index) const {
    return this->data_.at(index);
}

void print_binary_info(const Binary &binary) {
    std::printf("Binary info: %s %s %s, %s\n",
                file::binary_type_to_string(binary.type()).c_str(),
                file::bitness_to_string(binary.bitness()).c_str(),
                file::endianness_to_string(binary.endianness()).c_str(),
                file::arch_to_string(binary.architecture()).c_str());
}

} // namespace file
