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

static inline void fetch_decode_exec(DecodeExecState *s) {
  // fetch instruction
  // pc is 4 bytes(32bits)
  // val is the same as opcode1_0 because of union
  s->isa.instr.val = instr_fetch(&s->seq_pc, 4);
//   printf("cpu.pc:0x%x, s->isa.instr.val:0x%x\n", cpu.pc, s->isa.instr.val);
    // if (cpu.pc == 0x801000c0) {
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
