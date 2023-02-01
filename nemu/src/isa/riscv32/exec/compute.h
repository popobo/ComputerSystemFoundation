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

static inline def_EHelper(sltiu) {
    rtlreg_t value = *(id_src1->preg) < id_src2->imm ? 1 : 0;
    rtl_li(s, ddest, value);
    print_asm_template3(sltiu);
}

static inline def_EHelper(andi) {
    rtlreg_t value = *(id_src1->preg) & id_src2->simm;
    rtl_li(s, ddest, value);
    print_asm_template3(andi);
}

static inline def_EHelper(ori) {
    rtlreg_t value = *(id_src1->preg) | id_src2->simm;
    rtl_li(s, ddest, value);
    print_asm_template3(ori);
}

static inline def_EHelper(xori) {
    rtlreg_t value = *(id_src1->preg) ^ id_src2->simm;
    rtl_li(s, ddest, value);
    print_asm_template3(xori);
}