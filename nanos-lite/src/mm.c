#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
    void *result = pf;//(void *)((uintptr_t)pf + nr_page * PGSIZE);
    pf = (void *)((uintptr_t)pf + nr_page * PGSIZE);
    return result;
}


extern char _stack_top;
extern char _stack_pointer;
static inline void* pg_alloc(int bytes_num) {
    // AM make sure that n is always the multiple of PGSIZE
    void *pg = pf;//(void *)((uintptr_t)pf + bytes_num);
    
    printf("_stack_top:%x, \n", &_stack_top);
    printf("_stack_pointer:%x, \n", &_stack_pointer);
    printf("pg_alloc memset pg:%x, bytes_num:%d\n", (uint32_t)pg, bytes_num);
    memset(pg, 0, bytes_num);
    pf = (void *)((uintptr_t)pf + bytes_num);
    return pg;
}

void free_page(void *p) {
  panic("not implement yet");
}
/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
    printf("------------------------------------\n");
    printf("brk:%x, current->max_brk:%x\n", brk, current->max_brk);
    if (current->max_brk == 0) {
        current->max_brk = brk;
        printf("************************************\n\n\n\n\n");
        return 0;
    }

    uint32_t alloc_size = brk - current->max_brk;
    // how to allocate physical address
    uint32_t paup = (uint32_t)pg_alloc(alloc_size);
    uint32_t padown = paup + alloc_size;

    uint32_t vaup = (uint32_t)current->max_brk;
    uint32_t vadown = (uint32_t)brk;

    printf("paup:%x, padown:%x, vaup:%x, vadown:%x\n", paup, padown, vaup, vadown);

    while (padown <= paup) {
        map(&(current->as), (void *)vadown, (void *)padown, 1);
        vadown += PGSIZE;
        padown += PGSIZE;
    }

    current->max_brk = brk;
    printf("************************************\n\n\n\n\n\n");
    return 0;
}

void init_mm() {
    //set the heap start address provided by TRM as the first address of the free physical page 
    pf = (void *)ROUNDUP(heap.start, PGSIZE);
    Log("free physical pages starting from %x", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
