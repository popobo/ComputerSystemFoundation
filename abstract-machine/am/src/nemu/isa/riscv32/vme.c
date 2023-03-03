#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
    // 0x80000000 means mode bit is set
    asm volatile("csrw satp, %0" : : "r"(0x80000000 | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  // Remember here
  return satp << 12;
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

    uint32_t va_page_index = (uintptr_t)va / PGSIZE;
    uint32_t va_page_dir_index = va_page_index  >> VPN_LEN;

    PTE *pg_dir = (PTE *)as->ptr;
    struct PTE *dir_pte = (struct PTE *)(pg_dir + va_page_dir_index);
    if (dir_pte != NULL && 1 == dir_pte->PTE_uo.union_01.V) {
        
        PTE *pt = (PTE *)(dir_pte->PTE_uo.union_01.PPN_01 * PGSIZE);

        struct PTE *pt_pte = (struct PTE*)(pt + (va_page_index % PTE_NUM));
        
        if (0 == pt_pte->PTE_uo.union_01.V) {
            pt_pte->PTE_uo.val = ((uint32_t)pa / PGSIZE) << RSW_DAGUXWRV_LEN;
            // set DAGUXWRV
            pt_pte->PTE_uo.val |= PTE_V;
        } else {
            printf("pt_pte: %x is mapped \n", (uint32_t)pt_pte);
        }
    } else {
        PTE* new_pt = (PTE*)(pgalloc_usr(PGSIZE));
       
        struct PTE *pt_pte = (struct PTE*)(new_pt + (va_page_index % PTE_NUM));
        pt_pte->PTE_uo.val = ((uint32_t)pa / PGSIZE) << RSW_DAGUXWRV_LEN;
        // set DAGUXWRV
        pt_pte->PTE_uo.val |= PTE_V;
    
        dir_pte->PTE_uo.val = ((uint32_t)new_pt / PGSIZE) << RSW_DAGUXWRV_LEN;
        dir_pte->PTE_uo.val |= PTE_V;
    }
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
    assert(as != NULL);
    assert(kstack.end != NULL && kstack.start != NULL);

    Context * cp = (Context *)((uintptr_t)kstack.end - sizeof(Context));
    cp->epc = (uintptr_t)entry;
    cp->status = 0xc0100; //For DiffTest
    cp->cause = 0;
    for (int i = 0; i < sizeof(cp->gpr) / sizeof(cp->gpr[0]); ++i) {
        cp->gpr[i] = 0;
    }
    cp->pdir = as->ptr;

    return cp;
}
