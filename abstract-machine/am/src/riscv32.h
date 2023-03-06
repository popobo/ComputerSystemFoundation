#ifndef RISCV32_H__
#define RISCV32_H__

#include <stdint.h>

static inline uint8_t  inb(uintptr_t addr) { return *(volatile uint8_t  *)addr; }
static inline uint16_t inw(uintptr_t addr) { return *(volatile uint16_t *)addr; }
static inline uint32_t inl(uintptr_t addr) { return *(volatile uint32_t *)addr; }

static inline void outb(uintptr_t addr, uint8_t  data) { *(volatile uint8_t  *)addr = data; }
static inline void outw(uintptr_t addr, uint16_t data) { *(volatile uint16_t *)addr = data; }
static inline void outl(uintptr_t addr, uint32_t data) { *(volatile uint32_t *)addr = data; }

#define PTE_V 0x01
#define PTE_R 0x02
#define PTE_W 0x04
#define PTE_X 0x08
#define PTE_U 0x10

#define VPN_LEN (10)
#define RSW_DAGUXWRV_LEN (10)
#define PPN_0_LEN (10)
#define PPN_1_LEN (12)
#define PTE_LEN (32)
#define PTE_NUM (1024)
#define SATP_PPN_LEN (22)

// sstatus
#define SPIE 0b10000
#define SIE 0b10

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

#endif
