#include <memory.h>

static void *pf = NULL;

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
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %x", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
