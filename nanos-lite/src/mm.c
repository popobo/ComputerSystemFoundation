#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
    void *result = pf;
    pf = (void *)((uintptr_t)pf + nr_page * PGSIZE);
    return result;
}

static inline void* pg_alloc(int bytes_num) {
    // AM make sure that n is always the multiple of PGSIZE
    void *pg = pf;
    memset(pg, 0, bytes_num);
    pf = (void *)((uintptr_t)pf + bytes_num);
    return pg;
}

void free_page(void *p) {
  panic("not implement yet");
}

int mm_brk(uintptr_t brk) {    
    // printf("brk:%x, current->max_brk:%x\n", brk, current->max_brk);
    assert(brk >= current->max_brk);

    uint32_t alloc_size = brk - current->max_brk;
    // how to allocate physical address
    uint32_t pa_begin = (uint32_t)pg_alloc(alloc_size);
    uint32_t pa_end = pa_begin + alloc_size;

    uint32_t va_begin = (uint32_t)current->max_brk;
    // uint32_t va_end = (uint32_t)brk;

    
    while (pa_begin <= pa_end) {
        //printf("pa_begin:%x, pa_end:%x, va_begin:%x, va_end:%x\n", pa_begin, pa_end, va_begin, va_end);
        //printf("current:%x\n", (uint32_t)current);
        map(&(current->as), (void *)va_begin, (void *)pa_begin, 0);
        va_begin += PGSIZE;
        pa_begin += PGSIZE;
    }

    current->max_brk = brk;
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
