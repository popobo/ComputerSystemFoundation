#include <am.h>
#include <nemu.h>
#include <klib.h>

#define PT_BASE_ADDR_LR (10)
#define VPN_LEN (10)
#define PG_OFFSET_LEN (12)

union PTE {
    uint32_t V: 1;
    uint32_t R: 1;
    uint32_t W: 1;
    uint32_t X: 1;
    uint32_t U: 1;
    uint32_t G: 1;
    uint32_t A: 1;
    uint32_t D: 1;
    uint32_t RSW: 2;
    union  
    {
        struct 
        {    
            uint32_t PPN_0: 10;
            uint32_t PPN_1: 12;
        };
        uint32_t PPN_01: 22; 
    } PPN_01;
};

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  asm volatile("csrw satp, %0" : : "r"(0x80000000 | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << PG_OFFSET_LEN;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

    // kernal address space, address: pf, size: 4KB
  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
        //generate mapping relationship
      map(&kas, va, va, 0);
    }
  }

    //set the pnn of root page table
  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
    // this line alloc 4096 pa pages, total size is 4B, the size of PT, the same with kas
    // the size of memory that updirs points to is the same as the one of kas.ptr.
    // this is level 2 PT
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map updir to kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
    assert(as != NULL);
    // First, get the index of va
    // page index
    uint32_t va_page_index = (uintptr_t)va >> PG_OFFSET_LEN;
    // page directory index
    uint32_t va_page_dir_index = (uintptr_t)va >> (PG_OFFSET_LEN + VPN_LEN);

    PTE *pg_dir = (PTE *)as->ptr;
    union PTE *dir_pte = (union PTE *)(pg_dir + va_page_dir_index);
    
    // generate the level1 page table
    PTE* new_pt = (PTE*)(pgalloc_usr(PGSIZE));
    
    // set the physical page index in correspoding pte of new_pt
    union PTE *pg_pte = (union PTE*)(new_pt + (va_page_index % PTE_NUM));
    // set page frame base address
    pg_pte->PPN_01.PPN_01 = (uint32_t)pa >> PT_BASE_ADDR_LR;
    // set DAGUXWRV

    // because new_pt is aligned by 4KB, we just need the first 22 bits
    dir_pte->PPN_01.PPN_01 = (uint32_t)new_pt >> PT_BASE_ADDR_LR;
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
    assert(kstack.end != NULL && kstack.start != NULL);

    Context * cp = (Context *)((uintptr_t)kstack.end - sizeof(Context));
    cp->epc = (uintptr_t)entry;
    cp->status = 0xc0100; //For DiffTest
    cp->cause = 0;
    for (int i = 0; i < sizeof(cp->gpr) / sizeof(cp->gpr[0]); ++i) {
        cp->gpr[i] = 0;
    }

    return cp;
}
