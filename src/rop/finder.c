// rop/finder.c
#define _POSIX_C_SOURCE 200809L
#include "finder.h"

#include "../include/ropsmith/macros.h"
#include "../utils/utils.h"

#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int scan_elf_text_for_rets(const char* path, int context_bytes) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        perror("fopen");
        return -1;
    }

    // Check if the file is empty
    long filesize = get_file_size(f);
    if (filesize == 0) {
        fprintf(stderr, "Empty file: %s\n", path);
        fclose(f);
        return -1;
    }

    // Read ELF header
    Elf64_Ehdr ehdr;
    if (fread(&ehdr, 1, sizeof(ehdr), f) != sizeof(ehdr)) {
        perror("fread ehdr");
        fclose(f);
        return -1;
    }

    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "Not an ELF file: %s\n", path);
        fclose(f);
        return -1;
    }

    if (ehdr.e_ident[EI_CLASS] != ELFCLASS64) {
        fprintf(stderr, "Unsupported ELF class (only ELF64 supported)\n");
        fclose(f);
        return -1;
    }

    // Read section headers
    if (ehdr.e_shoff == 0 || ehdr.e_shnum == 0) {
        fprintf(stderr, "No section headers\n");
        fclose(f);
        return -1;
    }

    if (fseek(f, ehdr.e_shoff, SEEK_SET) != 0) {
        perror("fseek shoff");
        fclose(f);
        return -1;
    }

    Elf64_Shdr* sh_table = calloc(ehdr.e_shnum, sizeof(Elf64_Shdr));
    if (!sh_table) {
        perror("malloc sh_table");
        fclose(f);
        return -1;
    }

    if (fread(sh_table, sizeof(Elf64_Shdr), ehdr.e_shnum, f) != ehdr.e_shnum) {
        perror("fread sh_table");
        free(sh_table);
        fclose(f);
        return -1;
    }

    // Read section header string table to get names
    Elf64_Shdr shstr    = sh_table[ehdr.e_shstrndx];
    char*      shstrtab = malloc(shstr.sh_size);
    if (!shstrtab) {
        perror("malloc shstrtab");
        free(sh_table);
        fclose(f);
        return -1;
    }

    if (fseek(f, shstr.sh_offset, SEEK_SET) != 0) {
        perror("fseek shstr");
        free(shstrtab);
        free(sh_table);
        fclose(f);
        return -1;
    }

    if (fread(shstrtab, 1, shstr.sh_size, f) != shstr.sh_size) {
        perror("fread shstrtab");
        free(shstrtab);
        free(sh_table);
        fclose(f);
        return -1;
    }

    // Find .text section
    Elf64_Shdr* text_sh = NULL;
    for (int i = 0; i < ehdr.e_shnum; ++i) {
        const char* name = shstrtab + sh_table[i].sh_name;
        if (strcmp(name, ".text") == 0) {
            text_sh = &sh_table[i];
            break;
        }
    }

    if (!text_sh) {
        fprintf(stderr, "No .text section found\n");
        free(shstrtab);
        free(sh_table);
        fclose(f);
        return -1;
    }

    // Read .text
    unsigned long text_offset = text_sh->sh_offset;
    unsigned long text_size   = text_sh->sh_size;
    unsigned long text_addr   = text_sh->sh_addr; // virtual address

    if (fseek(f, text_offset, SEEK_SET) != 0) {
        perror("fseek text");
        free(shstrtab);
        free(sh_table);
        fclose(f);
        return -1;
    }

    unsigned char* text_buf = malloc(text_size);
    if (!text_buf) {
        perror("malloc text_buf");
        free(shstrtab);
        free(sh_table);
        fclose(f);
        return -1;
    }

    if (fread(text_buf, 1, text_size, f) != text_size) {
        perror("fread text_buf");
        free(text_buf);
        free(shstrtab);
        free(sh_table);
        fclose(f);
        return -1;
    }

    printf("Scanning %s : .text offset=0x%lx size=0x%lx vaddr=0x%lx\n",
           path,
           text_offset,
           text_size,
           text_addr);

    int ret_count = 0;
    for (size_t i = 0; i < text_size; ++i) {
        if (text_buf[i] == RET_OPCODE) {
            ret_count++;
            size_t ctx_start = (i >= (size_t)context_bytes) ? i - context_bytes : 0;
            size_t ctx_end   = i + 1; // include ret
            printf("GADGET (ret) at file_offset=0x%lx  vaddr=0x%lx\n",
                   (unsigned long)(text_offset + i),
                   (unsigned long)(text_addr + i));
            printf("Context (%zu bytes before):\n", i - ctx_start);
            print_bytes_hex(text_buf, ctx_start, ctx_end);
            printf("\n\n");
        }
    }

    free(text_buf);
    free(shstrtab);
    free(sh_table);
    fclose(f);
    return ret_count;
}
