#include <proc.h>
#include <elf.h>
#include <stdio.h>

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
    extern uint32_t ramdisk_start;
    size_t ramdisk_read(void *buf, size_t offset, size_t len);

    // now we found the ramdisk_start
    // how to extract the elf info from it
    Elf_Ehdr * ehdr = (Elf_Ehdr *)&ramdisk_start;
    Elf_Phdr * phdr = NULL;
    assert(*(uint32_t *)ehdr->e_ident == 0x464C457F);
    assert(ehdr->e_machine == EXPECT_TYPE);
    // printf("ehdr->e_type %d\n", ehdr->e_type);
    // printf("ehdr->e_machine: %d\n", ehdr->e_machine);
    // printf("ehdr->e_version: %d\n", ehdr->e_version);
    // printf("ehdr->e_phoff: %d\n", ehdr->e_phoff);
    // printf("ehdr->e_phentsize: %d\n", ehdr->e_phentsize);
    // printf("ehdr->e_phnum: %d\n", ehdr->e_phnum);
    assert(ehdr->e_phoff != 0 && ehdr->e_phnum != 0xffff);
    phdr = (Elf_Phdr *)((uintptr_t)ehdr + ehdr->e_phoff);
    for (int i = 0; i < ehdr->e_phnum; ++i) {
        // printf("phdr[%d].p_vaddr: 0x%x\n", i, (int64_t)phdr[i].p_vaddr);
        // printf("phdr[%d].p_offset: %d\n", i, phdr[i].p_offset);
        // printf("phdr[%d].p_memsz: %d\n", i, phdr[i].p_memsz);
        if (PT_LOAD == phdr[i].p_type) {
            ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_filesz);
            memset((void *)(phdr[i].p_vaddr + phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
        }
    }
    
    return ehdr->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = 0x%x", (int64_t)entry);
  ((void(*)())entry) ();
}

