#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"

static inline void set_width(DecodeExecState *s, int width) {
  if (width != 0) s->width = width;
}

static inline def_EHelper(load) {
  switch (s->isa.instr.i.funct3) {
    EXW(0b000, lds, 1); //lb
    EXW(0b001, lds, 2); //lh
    EXW(0b010, ld, 4) //lw
    EXW(0b100, ld, 1) //lbu
    EXW(0b101, ld, 2) //lhu
    default: exec_inv(s);
  }
}

static inline def_EHelper(store) {
  switch (s->isa.instr.s.funct3) {
    EXW(0b000, st, 1)
    EXW(0b001, st, 2)
    EXW(0b010, st, 4)
    default: exec_inv(s);
  }
}

static inline def_EHelper(irii) {
    switch (s->isa.instr.i.funct3)
    {
    EX(0b000, addi)
    EX(0b001, slli)
    EX(0b010, slti)
    EX(0b011, sltiu)
    EX(0b100, xori)
    EX(0b101, srlai)
    EX(0b110, ori)
    EX(0b111, andi)
    default: exec_inv(s);
    }
}

static inline def_EHelper(irro_func7_0) {
    switch (s->isa.instr.r.func3) {
    EX(0b000, add)
    EX(0b001, sll)
    EX(0b010, slt)
    EX(0b011, sltu)
    EX(0b100, xor)
    EX(0b101, srl)
    EX(0b110, or)
    EX(0b111, and)
    default: exec_inv(s);
    }
}

static inline def_EHelper(irro_func7_32) {
    switch (s->isa.instr.r.func3) {
    EX(0b000, sub)
    EX(0b101, sra)
    default: exec_inv(s);
    }
}


static inline def_EHelper(irro_func7_1) {
    switch (s->isa.instr.r.func3) {
    EX(0b000, mul) //mul s * s
    EX(0b001, mulh) //mulh us * us >> 32
    EX(0b010, mulhsu) //mulhsu
    EX(0b011, mulhu) //mulhu
    EX(0b100, div)
    EX(0b101, divu)
    EX(0b110, rem)
    EX(0b111, remu)
    default: exec_inv(s);
    }
}

// Integer Register-Register Operations
static inline def_EHelper(irro) {
    switch (s->isa.instr.r.func7)
    {
    EX(0b0000000, irro_func7_0)
    EX(0b0100000, irro_func7_32)
    EX(0b0000001, irro_func7_1)
    default: exec_inv(s);
    }
}

// Conditional Branches Instructions
static inline def_EHelper(cbi) {
    switch (s->isa.instr.b.func3)
    {
    EX(0b000, beq)
    EX(0b001, bne)
    EX(0b100, blt)
    EX(0b101, bge)
    EX(0b110, bltu)
    EX(0b111, bgeu)
    default: exec_inv(s);
    }
}

static inline def_EHelper(e_funcs) {
    switch (s->isa.instr.i.simm11_0)
    {
    EX(0b000000000000, ecall)
    EX(0b000100000010, sret)
    default: exec_inv(s);
    }
}

static inline def_EHelper(csrrw) {
    switch ((uint32_t)(s->isa.instr.i.simm11_0))
    {
    EX(0x100, csrrw_sstatus)
    EX(0x105, csrrw_stvec)
    EX(0x141, csrrw_sepc)
    EX(0x142, csrrw_scause)
    EX(0x180, csrrw_satp)
    default: exec_inv(s);
    }
}

static inline def_EHelper(csrrs) {
    switch ((uint32_t)(s->isa.instr.i.simm11_0))
    {
    EX(0x100, csrrs_sstatus)
    EX(0x105, csrrs_stvec)
    EX(0x141, csrrs_sepc)
    EX(0x142, csrrs_scause)
    EX(0x180, csrrs_satp)
    default: exec_inv(s);
    }
}

static inline def_EHelper(csrr) {
    switch (s->isa.instr.i.funct3)
    {
    EX(0b000, e_funcs)
    EX(0b001, csrrw)
    EX(0b010, csrrs)
    // EX(0b010, csrrs)
    // EX(0b011, csrrc)
    default: exec_inv(s);
    }
}

// cpu.pc:0x830004bc, s->isa.instr.val:0x50413
// cpu.pc:0x830004c0, s->isa.instr.val:0x2097
// cpu.pc:0x830004c4, s->isa.instr.val:0xb0c080e7 // 1011 0000 1100 00001  000  00001  1100111
// cpu.pc:0x82ffffd4, s->isa.instr.val:0x17f6bb15
// static int32_t count = 0;
// static int32_t count2 = 0;
static inline void fetch_decode_exec(DecodeExecState *s) {
    // fetch instruction
    // pc is 4 bytes(32bits)
    // val is the same as opcode1_0 because of union
    // if (cpu.satp > 0) {
    //     vaddr_read(0x80101a88, 4);
    // }
    s->isa.instr.val = instr_fetch(&s->seq_pc, 4);
    // if (cpu.pc > 0x83000000) {
    //     printf("cpu.pc:0x%x, s->isa.instr.val:0x%x\n", cpu.pc, s->isa.instr.val);
    // }
    
    // if (cpu.pc > 0x80101c44) {
    //     printf("cpu.gpr[9]._32:%x\n", cpu.gpr[9]._32);
    // }
    
    // if (cpu.gpr[9]._32 == 0x80103f88) {
    //     count++;
    // }

    // if (count == 485) {
    //     printf("---------\n");
    //     count2++;
    //     count = 0;
    // }

    // if (count2 == 2) {
    //     if (cpu.pc == 0x80101d14) {
    //         printf("cpu.pc:0x%x, s->isa.instr.val:0x%x\n", cpu.pc, s->isa.instr.val);
    //     }
    // }

    // if (cpu.satp > 0) {
    //     if (cpu.pc == 0x80101c58) {
    //         printf("cpu.pc:0x%x, s->isa.instr.val:0x%x\n", cpu.pc, s->isa.instr.val);
    //     }
    //     if (cpu.pc == 0x80101d10) {
    //         printf("cpu.pc:0x%x, s->isa.instr.val:0x%x\n", cpu.pc, s->isa.instr.val);
    //     }
    //     if (cpu.pc == 0x80101d14) {
    //         printf("cpu.pc:0x%x, s->isa.instr.val:0x%x\n", cpu.pc, s->isa.instr.val);
    //     }
    // }
    // if (cpu.pc == 0x80101d18) {
        
    //     if (cpu.gpr[9]._32 == 0x80103fa1) {
    //         printf("cpu1.pc:0x%x, s->isa.instr.val:0x%x, cpu.gpr[9]:%x\n", cpu.pc, s->isa.instr.val, cpu.gpr[9]._32);
    //     }
    //     printf("cpu.pc:0x%x, s->isa.instr.val:0x%x, cpu.gpr[9]:%x\n", cpu.pc, s->isa.instr.val, cpu.gpr[9]._32);
    // }
    // // 83007abc <_exit>:
    // // 83007abc:	00000893          	li	a7,0
    // if (cpu.pc == 0x83007abc) {
    //     printf("cpu.pc:0x%x, s->isa.instr.val:0x%x\n", cpu.pc, s->isa.instr.val);
    // }

    // // 83007aa4 <_syscall_>:
    // // 83007aa4:	00050893          	mv	a7,a0
    // if (cpu.pc == 0x83007aa4) {
    //     printf("cpu.pc:0x%x, s->isa.instr.val:0x%x\n", cpu.pc, s->isa.instr.val);
    // }

    Assert(s->isa.instr.i.opcode1_0 == 0x3, "Invalid instruction");
    switch (s->isa.instr.i.opcode6_2) {
        IDEX (0b00000, I, load)
        IDEX (0b01000, S, store)
        IDEX (0b01101, U, lui)
        IDEX (0b00101, U, auipc)
        IDEX (0b00100, I, irii)
        IDEX (0b01100, R, irro)
        IDEX (0b11011, J, jal)
        IDEX (0b11001, I, jalr)
        IDEX (0b11000, B, cbi)
        IDEX (0b11100, I, csrr)
        EX   (0b11010, nemu_trap)
        default: exec_inv(s);
    }
}

static inline void reset_zero() {
  reg_l(0) = 0;
}

vaddr_t isa_exec_once() {
  DecodeExecState s;
  s.is_jmp = 0;
  s.seq_pc = cpu.pc;

  fetch_decode_exec(&s);
  update_pc(&s);

  reset_zero();

  return s.seq_pc;
}
