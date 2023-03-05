#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

#define PT_BASE_ADDR_LR (12)
#define PG_OFFSET_LEN (12)
#define MODE_POS (0x80000000)
#define LEVELS (2)
#define ETRA_PDA (0x3fffff)

#define VPN_LEN (10)
#define RSW_DAGUXWRV_LEN (10)
#define PPN_0_LEN (10)
#define PPN_1_LEN (12)
#define PTE_LEN (32)
#define PTE_NUM (1024)

typedef uint32_t PTE;

struct PTE {
    union {
        struct {
            uint32_t V: 1;
            uint32_t R: 1;
            uint32_t W: 1;
            uint32_t X: 1;
            uint32_t U: 1;
            uint32_t G: 1;
            uint32_t A: 1;
            uint32_t D: 1;
            uint32_t RSW: 2;
            uint32_t PPN_01: 22; 
        } union_01;
        struct {
            uint32_t V: 1;
            uint32_t R: 1;
            uint32_t W: 1;
            uint32_t X: 1;
            uint32_t U: 1;
            uint32_t G: 1;
            uint32_t A: 1;
            uint32_t D: 1;
            uint32_t RSW: 2;
            uint32_t PPN_0: 10;
            uint32_t PPN_1: 12;
        } union_0_1;
        uint32_t val;
    } PTE_uo;
};

struct VA {
    union 
    {
        struct {
            uint32_t page_offset: 12;
            uint32_t VPN_0: 10;
            uint32_t VPN_1: 10;            
        } fields;
        uint32_t val;
    } va_no;
};

struct PA {
    union 
    {
        struct {
            uint32_t page_offset: 12;
            uint32_t PPN_01: 20;      
        } fields;
        uint32_t val;
    } pa_no;
};


int isa_vaddr_check(vaddr_t vaddr, int type, int len) {
    uint32_t mode = cpu.satp & MODE_POS;
    return MODE_POS == mode ? MEM_RET_NEED_TRANSLATE : MEM_RET_OK;
}

paddr_t isa_mmu_translate(vaddr_t addr, int type, int len) {
    uint32_t page_dir_address = (cpu.satp & ETRA_PDA) * PAGE_SIZE;
    uintptr_t level = LEVELS - 1;

    struct VA va = {};
    va.va_no.val = addr;
    
    struct PTE pte_page_table = {};
    
    pte_page_table.PTE_uo.val = paddr_read(page_dir_address + va.va_no.fields.VPN_1 * 4, 4);

    assert(pte_page_table.PTE_uo.val != 0);
    assert(pte_page_table.PTE_uo.union_01.V == 1);

    level -= 1;
    uint32_t page_table_address = pte_page_table.PTE_uo.union_01.PPN_01 * PAGE_SIZE;

    struct PTE pte_page_frame = {};
    pte_page_frame.PTE_uo.val = paddr_read(page_table_address + va.va_no.fields.VPN_0 * sizeof(PTE), sizeof(PTE));   

    assert(pte_page_frame.PTE_uo.union_01.V == 1);
    
    struct PA pa = {};
    pa.pa_no.fields.PPN_01 = pte_page_frame.PTE_uo.union_01.PPN_01;
    pa.pa_no.fields.page_offset = va.va_no.fields.page_offset;

    // assert(addr == pa.pa_no.val);

    return pa.pa_no.val;
}
