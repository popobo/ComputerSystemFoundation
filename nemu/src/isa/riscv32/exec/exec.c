#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"

static inline void set_width(DecodeExecState *s, int width) {
  if (width != 0) s->width = width;
}

static inline def_EHelper(load) {
  switch (s->isa.instr.i.funct3) {
    EXW  (2, ld, 4)
    default: exec_inv(s);
  }
}

static inline def_EHelper(store) {
  switch (s->isa.instr.s.funct3) {
    EXW  (2, st, 4)
    default: exec_inv(s);
  }
}

static inline def_EHelper(irii) {
    switch (s->isa.instr.i.funct3)
    {
        EX(0b000, addi)
        EX(0b010, slti)
        EX(0b011, sltiu)
        EX(0b100, xori)
        EX(0b110, ori)
        EX(0b111, andi)
        default:
            exec_inv(s);
    }
}

static inline def_EHelper(irro_) {
    switch (s->isa.instr.r.func7)
    {
        EX(0b0000000, add)
        EX(0b0100000, sub)
        default:
            break;
    }
}

// Integer Register-Register Operations
static inline def_EHelper(irro) {
    switch (s->isa.instr.r.func3)
    {
        EX(0b000, irro_)
        EX(0b010, slt)
        default:
            exec_inv(s);
    }
}

static inline void fetch_decode_exec(DecodeExecState *s) {
  // fetch instruction
  // pc is 4 bytes(32bits)
  // val is the same as opcode1_0 because of union
  s->isa.instr.val = instr_fetch(&s->seq_pc, 4);
  Assert(s->isa.instr.i.opcode1_0 == 0x3, "Invalid instruction");
  switch (s->isa.instr.i.opcode6_2) {
    IDEX (0b00000, I, load)
    IDEX (0b01000, S, store)
    IDEX (0b01101, U, lui)
    IDEX (0b00101, U, auipc)
    IDEX (0b00100, I, irii)
    IDEX (0b01100, R, irro)
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
