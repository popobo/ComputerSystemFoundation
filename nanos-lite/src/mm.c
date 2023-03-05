#include <memory.h>
#include <proc.h>

static void *pf = NULL;
extern PCB *current;

void* new_page(size_t nr_page) {
    void *result = pf;
    pf = (void *)((uintptr_t)pf + nr_page * PGSIZE);
    return result;
}

static inline void* pg_alloc(int n) {
    assert(n % PGSIZE == 0);
    void *result = new_page(n / PGSIZE);
    memset(result, 0, n);
    return result;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
    assert(current->max_brk != 0);

    if (current->max_brk >= brk) {
        return 0;
    }

    uint32_t alloc_size = brk - current->max_brk;
    uint32_t alloc_pages = alloc_size / PGSIZE;

    // if alloc_pages++, there will be a operation undefined error
    alloc_pages = alloc_size % PGSIZE > 0 ? alloc_pages + 1 : alloc_pages;
    
    uint32_t alloc_pa_space_begin = (uint32_t)new_page(alloc_pages);
    
    uint32_t pa = alloc_pa_space_begin;
    uint32_t va = current->max_brk;

    for (int i = 0; i < alloc_pages; ++i) {
        map(&(current->as), (void *)(va + i * PGSIZE), (void *)(pa + i * PGSIZE), 0);
    }

    current->max_brk = brk;

    return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %x", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
