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
    assert(ehdr.e_phoff != 0 && ehdr.e_phnum != 0xffff);
    for (int i = 0; i < ehdr.e_phnum; ++i) {
        fs_lseek(fd, ehdr.e_phoff + sizeof(Elf_Phdr) * i, SEEK_SET);
        fs_read(fd, &phdr, sizeof(Elf_Phdr));
        
        if (PT_LOAD != phdr.p_type) {
            continue;
        }
        
        fs_lseek(fd, phdr.p_offset, SEEK_SET);
        fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
        memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
    }

    return ehdr.e_entry;
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
    assert(pcb != NULL);
    assert(entry != NULL);

    Area kstack;
    kstack.start = (void *)&pcb->stack[0];
    kstack.end = (void *)&pcb->stack[STACK_SIZE];

    pcb->cp = kcontext(kstack, entry, arg);
}

void context_uload(PCB *pcb, const char *filename) {
    uintptr_t entry = loader(pcb, filename);
    assert(pcb != NULL);
    assert(filename != NULL);

    Area kstack;
    kstack.start = (void *)&pcb->stack[0];
    kstack.end = (void *)&pcb->stack[STACK_SIZE];

    pcb->cp = ucontext(NULL, kstack, (void *)entry);
    // use heap.end as the stack top of user process, and put it in GPRx according to the convention
    pcb->cp->GPRx = (uintptr_t)heap.end;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = 0x%x", (int64_t)entry);
  ((void(*)())entry) ();
}

