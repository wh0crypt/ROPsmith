//===----------------------------------------------------------------------===//
//
// Part of the ROPsmith Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file core/rop/scanner.cpp
/// Implementation of functions for scanning ELF binaries and detecting ROP
/// gadgets.
///
//===----------------------------------------------------------------------===//

#include "scanner.h"

#include "utils/io.h"

#include <cinttypes>
#include <cstddef>
#include <memory>

namespace rop {

void print_section_info(uint64_t offset, uint64_t size, uint64_t addr) {
    std::printf(".text offset=0x%" PRIx64 " size=0x%" PRIx64 " vaddr=0x%" PRIx64
                "\n\n",
                offset,
                size,
                addr);
}

std::size_t find_ret_instructions(const std::filesystem::path &path,
                                  int                          context_bytes) {
    // Check if the file is empty
    std::uintmax_t filesize = 0;

    try {
        filesize = std::filesystem::file_size(path);
    } catch (const std::filesystem::filesystem_error &e) {
        throw std::runtime_error(std::string("filesystem_error: ") + e.what());
    }

    if (filesize == 0) {
        throw std::runtime_error("file is empty");
    }

    // Read ELF header
    Elf64_Ehdr                                        ehdr;
    std::unique_ptr<std::FILE, utils::io::FileCloser> fp(
        utils::io::open_binary(path));
    if (!fp) {
        throw std::runtime_error("fopen error: could not open file");
    }

    if (std::fread(&ehdr, 1, sizeof(ehdr), fp.get()) != sizeof(ehdr)) {
        throw std::runtime_error("fread error: could not read ELF header");
    }

    if (ehdr.e_ident[EI_CLASS] != ELFCLASS64) {
        throw std::runtime_error(
            "unsupported ELF class (only ELF64 supported)");
    }

    // Read section headers
    if (ehdr.e_shoff == 0 || ehdr.e_shnum == 0) {
        throw std::runtime_error("no section headers found");
    }

    if (std::memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        throw std::runtime_error("not an ELF file");
    }

    if (std::fseek(fp.get(), ehdr.e_shoff, SEEK_SET) != 0) {
        throw std::runtime_error(
            "fseek error: could not seek to section headers");
    }

    std::unique_ptr<Elf64_Shdr[]> sh_table(new Elf64_Shdr[ehdr.e_shnum]());
    if (!sh_table) {
        throw std::runtime_error(
            "new error: could not allocate memory for section headers");
    }

    if (std::fread(
            sh_table.get(), sizeof(Elf64_Shdr), ehdr.e_shnum, fp.get()) !=
        ehdr.e_shnum) {
        throw std::runtime_error("fread error: could not read section headers");
    }

    // Read section header string table to get names
    Elf64_Shdr              shstr = sh_table[ehdr.e_shstrndx];
    std::unique_ptr<char[]> shstrtab(new char[shstr.sh_size]());
    if (!shstrtab) {
        throw std::runtime_error("new error: could not allocate memory for "
                                 "section header string table");
    }

    if (std::fseek(fp.get(), shstr.sh_offset, SEEK_SET) != 0) {
        throw std::runtime_error(
            "fseek error: could not seek to section header string table");
    }

    if (std::fread(shstrtab.get(), 1, shstr.sh_size, fp.get()) !=
        shstr.sh_size) {
        throw std::runtime_error(
            "fread error: could not read section header string table");
    }

    // Find .text section
    Elf64_Shdr *text_sh = NULL;
    for (int i = 0; i < ehdr.e_shnum; ++i) {
        const char *name = shstrtab.get() + sh_table[i].sh_name;
        if (!std::strcmp(name, ".text")) {
            text_sh = &sh_table[i];
            break;
        }
    }

    if (!text_sh) {
        throw std::runtime_error("no .text section found");
    }

    // Read .text
    Elf64_Off   text_offset = text_sh->sh_offset;
    Elf64_Xword text_size   = text_sh->sh_size;
    Elf64_Addr  text_addr   = text_sh->sh_addr; // virtual address

    if (std::fseek(fp.get(), text_offset, SEEK_SET) != 0) {
        throw std::runtime_error(
            "fseek error: could not seek to .text section");
    }

    std::unique_ptr<std::byte[]> text_buf(
        static_cast<std::byte *>(new std::byte[text_size]));
    if (!text_buf) {
        throw std::runtime_error(
            "new error: could not allocate memory for .text section");
    }

    if (std::fread(text_buf.get(), 1, text_size, fp.get()) != text_size) {
        throw std::runtime_error("fread error: could not read .text section");
    }

    print_section_info(text_offset, text_size, text_addr);

    std::size_t ret_count = 0;
    for (std::size_t i = 0; i < text_size; ++i) {
        if (text_buf[i] == RET_OPCODE) {
            ret_count++;
            std::size_t ctx_start =
                (i >= (std::size_t)context_bytes) ? i - context_bytes : 0;
            std::size_t ctx_end = i + 1; // include ret
            std::printf("GADGET (ret) at file_offset=0x%" PRIx64
                        "  vaddr=0x%" PRIx64 "\n",
                        text_offset + i,
                        text_addr + i);
            std::printf("Context (%zu bytes before):\n", i - ctx_start);
            utils::io::print_bytes_hex(text_buf, ctx_start, ctx_end);
            std::printf("\n\n");
        }
    }

    return ret_count;
}

} // namespace rop
