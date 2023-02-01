static inline def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
  print_asm_template2(lui);
}

static inline def_EHelper(auipc) {
    rtl_addi(s, (&s->seq_pc), (&s->seq_pc), id_src1->imm);
    print_asm_template2(auipc);
}

static inline def_EHelper(addi) {
    rtl_addi(s, ddest, dsrc1, id_src2->simm);
    print_asm_template3(addi);
}

static inline def_EHelper(slti) {
    int rs1 = *(id_src1->preg);
    rtlreg_t value = rs1 < id_src2->simm ? 1 : 0;
    rtl_li(s, ddest, value);
    print_asm_template3(slti);
}