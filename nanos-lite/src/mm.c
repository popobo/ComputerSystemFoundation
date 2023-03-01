#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
    return (void *)((uintptr_t)pf + nr_page * PGSIZE);
}

static inline void* pg_alloc(int n) {
    // AM make sure that n is always the multiple of PGSIZE
    void *pg = (void *)((uintptr_t)pf + n);
    memset(pg, 0, n);
    return pg;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
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
