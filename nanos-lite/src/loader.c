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

static uintptr_t loader(PCB *pcb, const char *filename) {
    extern uint32_t ramdisk_start;
    // now we found the ramdisk_start
    // how to extract the elf info from it
    Elf32_Ehdr * ehdr = (Elf32_Ehdr *)&ramdisk_start;
    Elf32_Phdr * phdr = NULL;
    assert(*(uint32_t *)ehdr->e_ident == 0x464C457F);
    printf("ehdr->e_type %d\n", ehdr->e_type);
    printf("ehdr->e_machine: %d\n", ehdr->e_machine);
    printf("ehdr->e_version: %d\n", ehdr->e_version);
    printf("ehdr->e_phoff: %d\n", ehdr->e_phoff);
    printf("ehdr->e_phentsize: %d\n", ehdr->e_phentsize);
    printf("ehdr->e_phnum: %d\n", ehdr->e_phnum);
    assert(ehdr->e_phoff != 0);
    phdr = (Elf32_Phdr *)((uint32_t)ehdr + ehdr->e_phoff);
    for (int i = 0; i < ehdr->e_phnum; ++i) {
        printf("phdr[%d].p_vaddr: 0x%x\n", i, phdr[i].p_vaddr);
        printf("phdr[%d].p_offset: %d\n", i, phdr[i].p_offset);
        printf("phdr[%d].p_memsz: %d\n", i, phdr[i].p_memsz);
    }

    return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

