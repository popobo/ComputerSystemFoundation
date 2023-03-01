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
    // printf("__FILE__:%s, __LINE__:%d\n", __FILE__, __LINE__);
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << SATP_PPN_LEN;
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
    printf("__FILE__:%s, __LINE__:%d\n", __FILE__, __LINE__);
    //set the pnn of root page table
  set_satp(kas.ptr);
    printf("__FILE__:%s, __LINE__:%d\n", __FILE__, __LINE__);
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

static inline void print_pte(struct PTE *pte, const char *pte_type) {
    printf("%s\n", pte_type);
    printf("pte->PTE_uo.val: %x\n", pte->PTE_uo.val);
    printf("pte->PTE_uo.union_01.V:%d\n", pte->PTE_uo.union_01.V);
    printf("pte->PTE_uo.union_0_1.PPN_1): 0x%x\n", pte->PTE_uo.union_0_1.PPN_1);
    printf("pte->PTE_uo.union_0_1.PPN_0: 0x%x\n", pte->PTE_uo.union_0_1.PPN_0);
    printf("pte->PTE_uo.union_01.PPN_01: 0x%x\n", pte->PTE_uo.union_01.PPN_01);

    printf("pte->PTE_uo.union_0_1.PPN_1): %d\n", pte->PTE_uo.union_0_1.PPN_1);
    printf("pte->PTE_uo.union_0_1.PPN_0: %d\n", pte->PTE_uo.union_0_1.PPN_0);
    printf("pte->PTE_uo.union_01.PPN_01: %d\n", pte->PTE_uo.union_01.PPN_01);
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
    assert(as != NULL);
    // printf("--------------------------------------------\n");
    // printf("as->ptr:%x, (uintptr_t)va: %x, (uintptr_t)pa: %x\n", (uint32_t)as->ptr, (uintptr_t)va, (uintptr_t)pa);
    // First, get the index of va
    // page index
    uint32_t va_page_index = (uintptr_t)va / PGSIZE;
    // printf("va_page_index: %d\n", va_page_index);
    // page directory index
    uint32_t va_page_dir_index = va_page_index  >> VPN_LEN;
    // printf("va_page_dir_index: %d\n", va_page_dir_index);
    // printf("index in page_table: %d\n", va_page_index % PTE_NUM);

    PTE *pg_dir = (PTE *)as->ptr;
    struct PTE *dir_pte = (struct PTE *)(pg_dir + va_page_dir_index);
    if (dir_pte != NULL && 1 == dir_pte->PTE_uo.union_01.V) {
        
        PTE *pt = (PTE *)(dir_pte->PTE_uo.union_01.PPN_01 * PGSIZE);
        // printf("pt:%x, (va_page_index % PTE_NUM:%d\n", pt, (va_page_index % PTE_NUM));
        struct PTE *pt_pte = (struct PTE*)(pt + (va_page_index % PTE_NUM));
        //printf("pt_pte: %x\n", pt_pte);
        assert(pt_pte->PTE_uo.union_01.V == 0);
        pt_pte->PTE_uo.val = ((uint32_t)pa / PGSIZE) << RSW_DAGUXWRV_LEN;
        // set DAGUXWRV
        pt_pte->PTE_uo.val |= PTE_V;
    } else {
        // printf("pg_dir + va_page_dir_index:%x\n", pg_dir + va_page_dir_index);
    
        // generate the level1 page table
        PTE* new_pt = (PTE*)(pgalloc_usr(PGSIZE));
        // printf("new_pt:%x\n", new_pt);
        
        // set the physical page index in correspoding pte of new_pt
        struct PTE *pt_pte = (struct PTE*)(new_pt + (va_page_index % PTE_NUM));
        // printf("new_pt + (va_page_index PTE_NUM):%x\n", new_pt + (va_page_index % PTE_NUM));
        // set page frame base address
        pt_pte->PTE_uo.val = ((uint32_t)pa / PGSIZE) << RSW_DAGUXWRV_LEN;
        // set DAGUXWRV
        pt_pte->PTE_uo.val |= PTE_V;

        // print_pte(pt_pte, "PAGE PTE");

        // because new_pt is aligned by 4KB, we just need the first 22 bits
        dir_pte->PTE_uo.val = ((uint32_t)new_pt / PGSIZE) << RSW_DAGUXWRV_LEN;
        dir_pte->PTE_uo.val |= PTE_V;

        
    }
    
    // print_pte(dir_pte, "PAGE DIRECTORY PTE");
    // if ((uint32_t)va < 0x80101cb4) {
    //     printf("val:%d\n", *(uint32_t *)(pg_dir + va_page_dir_index));
    //     printf("va_page_dir_index:%d\n", va_page_dir_index);
    //     printf("pg_dir + va_page_dir_index:%x\n", pg_dir + va_page_dir_index);
    //     printf("(va_page_index % PTE_NUM):%d\n", (va_page_index % PTE_NUM));
    //     print_pte(pt_pte, "PAGE PTE");
    //     print_pte(dir_pte, "PAGE DIRECTORY PTE");
    // }
    // printf("*********************************************\n");
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
