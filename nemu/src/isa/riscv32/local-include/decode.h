#include <cpu/exec.h>
#include "rtl.h"

static inline void sext(rtlreg_t *temp, uint32_t width) {
    int32_t num = *temp;
    assert(width >= 0 && width <= 32);
    num = (num) << width;
    num = (num) >> width;
    *temp = num;
}

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (DecodeExecState *s, Operand *op, word_t val, bool load_val)

static inline def_DopHelper(i) {
  op->type = OP_TYPE_IMM;
  op->imm = val;

  print_Dop(op->str, OP_STR_SIZE, "%d", op->imm);
}

static inline def_DopHelper(r) {
  op->type = OP_TYPE_REG;
  op->reg = val; // this is the index of reg
  op->preg = &reg_l(val); //reg(val) is reg and the value of reg

  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg));
}

static inline def_DHelper(I) {
  decode_op_r(s, id_src1, s->isa.instr.i.rs1, true);
  decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, true);
  decode_op_r(s, id_dest, s->isa.instr.i.rd, false);
}

static inline def_DHelper(U) {
  decode_op_i(s, id_src1, s->isa.instr.u.imm31_12 << 12, true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, false);

  print_Dop(id_src1->str, OP_STR_SIZE, "0x%x", s->isa.instr.u.imm31_12);
}

static inline def_DHelper(S) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, true);
  sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
  decode_op_i(s, id_src2, simm, true);
  decode_op_r(s, id_dest, s->isa.instr.s.rs2, true);
}

static inline def_DHelper(R) {
    decode_op_r(s, id_dest, s->isa.instr.r.rd, true);
    decode_op_r(s, id_src1, s->isa.instr.r.rs1, true);
    decode_op_r(s, id_src2, s->isa.instr.r.rs2, true);
}

static inline def_DHelper(J) {
    *t0 = (s->isa.instr.j.simm20 << 20) | 
          (s->isa.instr.j.imm19_12 << 12) | 
          (s->isa.instr.j.imm11 << 11) | 
          (s->isa.instr.j.imm10_1 << 1);
    
    sext(t0, 11);

    decode_op_i(s, id_src1, *t0, true);
    decode_op_r(s, id_dest, s->isa.instr.j.rd, true);

    print_Dop(id_src1->str, OP_STR_SIZE, "0x%x", *t0);
}

static inline def_DHelper(B) {
    id_dest->imm = (s->isa.instr.b.imm12 << 12) |
                   (s->isa.instr.b.imm11 << 11) |
                   (s->isa.instr.b.imm10_5 << 5)|
                   (s->isa.instr.b.imm4_1 << 1);
    
    sext(&id_dest->imm, 19);
    
    decode_op_r(s, id_src1, s->isa.instr.b.rs1, true);
    decode_op_r(s, id_src2, s->isa.instr.b.rs2, true);
}