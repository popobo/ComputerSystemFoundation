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

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
    uintptr_t entry = loader(pcb, filename);
    assert(pcb != NULL);
    assert(filename != NULL);

    Area kstack;
    kstack.start = (void *)&pcb->stack[0];
    kstack.end = (void *)&pcb->stack[STACK_SIZE];

    pcb->cp = ucontext(NULL, kstack, (void *)entry);
    // use heap.end as the stack top of user process, and put it in GPRx according to the convention
    
    uintptr_t ustack_top = (uintptr_t)heap.end;
    size_t argv_num = 0;
    size_t envp_num = 0;
    size_t str_len = 0;
    while(envp[envp_num++] != NULL) {};
    while(argv[argv_num++] != NULL) {};
    envp_num--;
    argv_num--;

    uintptr_t envp_str_pos[envp_num + 1];
    uintptr_t argv_str_pos[argv_num + 1];
    
    for (int i = 0; i < envp_num; ++i) {
        str_len = strlen(envp[i]) + 1;
        ustack_top -= str_len;
        memcpy((void *)ustack_top, envp[i], str_len);
        envp_str_pos[i] = ustack_top;
    }
    envp_str_pos[envp_num] = 0;

    for (int i = 0; i < argv_num; ++i) {
        str_len = strlen(argv[i]) + 1;
        ustack_top -= str_len;
        memcpy((void *)ustack_top, argv[i], str_len);
        printf("ustack_top:%x\n", ustack_top);
        argv_str_pos[i] = ustack_top;
    }
    argv_str_pos[argv_num] = 0;
    
    for (int i = envp_num; i > -1; --i) {
        ustack_top -= sizeof(uintptr_t);
        *(uintptr_t *)ustack_top = envp_str_pos[i];
    }

    for (int i = argv_num; i > -1; --i) {
        ustack_top -= sizeof(uintptr_t);
        *(uintptr_t *)ustack_top = argv_str_pos[i];
    }

    ustack_top -= sizeof(int32_t);
    *(int32_t *)ustack_top = argv_num;

    pcb->cp->GPRx = ustack_top; // ustack.end

    printf("ustack_top:%x\n", ustack_top);
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = 0x%x", (int64_t)entry);
  ((void(*)())entry) ();
}

