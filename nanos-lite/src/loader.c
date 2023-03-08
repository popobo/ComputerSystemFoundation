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

#define USER_STACK_PAGES 8

static uintptr_t loader(PCB *pcb, const char *filename) {
    assert(filename != NULL);
    
    int fd = fs_open(filename, 0, 0);
    if (fd < 0) {
        return 0;
    }

    Elf_Ehdr ehdr = {};
    Elf_Phdr phdr = {};
    
    fs_lseek(fd, 0, SEEK_SET);
    fs_read(fd, (void *)&ehdr, sizeof(Elf_Ehdr));
    assert(*(uint32_t *)ehdr.e_ident == 0x464C457F);
    assert(ehdr.e_machine == EXPECT_TYPE);
    assert(ehdr.e_phoff != 0 && ehdr.e_phnum != 0xffff);
    
    uint32_t va_end_ptr = 0;
    for (int i = 0; i < ehdr.e_phnum; ++i) {
        fs_lseek(fd, ehdr.e_phoff + sizeof(Elf_Phdr) * i, SEEK_SET);
        fs_read(fd, &phdr, sizeof(Elf_Phdr));
        
        if (PT_LOAD != phdr.p_type) {
            continue;
        }
        

        fs_lseek(fd, phdr.p_offset, SEEK_SET);
        uint32_t unload_filesz = phdr.p_filesz;
        while (unload_filesz > 0) {
            // for a new phdr that needs to be load
            void *va = (void *)(phdr.p_vaddr + (phdr.p_filesz - unload_filesz));
            // allocate a new page frame in pa
            void *pa = new_page(1);
            // how many bytes should be loaded this time
            uint32_t load_len = unload_filesz > PGSIZE ? PGSIZE : unload_filesz;
            // read load_len bytes from elf file into new page frame
            uint32_t read_len = fs_read(fd, pa, load_len);
            // size of the unloaded part
            unload_filesz -= read_len;

            if (load_len < PGSIZE) {
                va_end_ptr = (uint32_t)va + load_len;
                memset((void *)((uint32_t)pa + load_len), 0, PGSIZE - load_len);
            }

            // map va to pa
            map(&(pcb->as), va, pa, 0);
        }
        
        uint32_t original_unload_memsz = phdr.p_memsz - phdr.p_filesz;
        uint32_t unload_memsz = original_unload_memsz;
        while (unload_memsz > 0) {
            void *va = (void *)(va_end_ptr + (original_unload_memsz - unload_memsz));
            void *pa = new_page(1);
            memset(pa, 0, PGSIZE);
            
            uint32_t load_len = unload_memsz > PGSIZE ? PGSIZE : unload_memsz;
            unload_memsz -= load_len;

            if (load_len < PGSIZE) {
                va_end_ptr = (uint32_t)va + load_len;        
            }

            map(&(pcb->as), va, pa, 0);
        }
    }
    
    pcb->max_brk = va_end_ptr;

    fs_close(fd);

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

int32_t context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
    assert(pcb != NULL);
    assert(filename != NULL);

    // as->ptr page_directory_address 
    // pcb->as.start 0x40000000
    // pcb->as.end 0x80000000
    protect(&(pcb->as));
    uint32_t ustack_begin = (uint32_t)new_page(USER_STACK_PAGES);
    uint32_t ustack_end = ustack_begin + USER_STACK_PAGES * PGSIZE;
    uint32_t ustack_top = ustack_end;

    // Remember here, I treat ustack_begin as the top of ustack by mistake.

    for (int i = USER_STACK_PAGES; i > 0; --i) {
        map(&(pcb->as), (void *)((uint32_t)pcb->as.area.end - i * PGSIZE), (void *)(ustack_top - i * PGSIZE), 0);
    }
    
    size_t argv_num = 0;
    size_t envp_num = 0;
    size_t str_len = 0;
    if (envp != NULL) {
        while(envp[envp_num++] != NULL) {};
        envp_num--;
    }
    
    if (argv != NULL) {
        while(argv[argv_num++] != NULL) {};
        argv_num--;
    }

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

    uintptr_t entry = loader(pcb, filename);
    if (0 == entry) {
        return -1;
    }

    Area kstack;
    kstack.start = (void *)&pcb->stack[0];
    kstack.end = (void *)&pcb->stack[STACK_SIZE];
    pcb->cp = ucontext(&(pcb->as), kstack, (void *)entry);
    // set a[0] the stack tops
    pcb->cp->GPRx = (uint32_t)pcb->as.area.end - (ustack_end - ustack_top);

    return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = 0x%x", (int64_t)entry);
  ((void(*)())entry) ();
}

