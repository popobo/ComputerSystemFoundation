#include <proc.h>
#include <elf.h>
#include <stdio.h>
#include "fs.h"

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
#define EXPECT_TYPE EM_RISCV
#else
# error unsupported ISA __ISA__
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
    assert(filename != NULL);
    
    int fd = fs_open(filename, 0, 0);
    Elf_Ehdr ehdr = {};
    Elf_Phdr phdr = {};

    fs_read(fd, (void *)&ehdr, sizeof(Elf_Ehdr));
    assert(*(uint32_t *)ehdr.e_ident == 0x464C457F);
    assert(ehdr.e_machine == EXPECT_TYPE);
    // printf("ehdr.e_type %d\n", ehdr.e_type);
    // printf("ehdr.e_machine: %d\n", ehdr.e_machine);
    // printf("ehdr.e_version: %d\n", ehdr.e_version);
    // printf("ehdr.e_phoff: %d\n", ehdr.e_phoff);
    // printf("ehdr.e_phentsize: %d\n", ehdr.e_phentsize);
    // printf("ehdr.e_phnum: %d\n", ehdr.e_phnum);
    // printf("ehdr.e_entry: 0x%x\n", (int64_t)ehdr.e_entry);
    assert(ehdr.e_phoff != 0 && ehdr.e_phnum != 0xffff);
    for (int i = 0; i < ehdr.e_phnum; ++i) {
        fs_lseek(fd, ehdr.e_phoff + sizeof(Elf_Phdr) * i, SEEK_SET);
        fs_read(fd, &phdr, sizeof(Elf_Phdr));
        
        if (PT_LOAD != phdr.p_type) {
            continue;
        }

        // printf("phdr.p_vaddr: 0x%x\n", (int64_t)phdr.p_vaddr);
        // printf("phdr.p_offset: %d\n", phdr.p_offset);
        // printf("phdr.p_filesz: %d\n", phdr.p_filesz);
        // printf("phdr.p_memsz: %d\n", phdr.p_memsz);
        
        fs_lseek(fd, phdr.p_offset, SEEK_SET);
        fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
        memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
    }

    return ehdr.e_entry;
}


void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = 0x%x", (int64_t)entry);
  ((void(*)())entry) ();
}

