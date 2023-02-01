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
    *ddest = (*(dsrc1) < id_src2->simm) ? 1 : 0;
    print_asm_template3(slti);
}

static inline def_EHelper(sltiu) {
    *ddest = *(id_src1->preg) < id_src2->imm ? 1 : 0;
    print_asm_template3(sltiu);
}

static inline def_EHelper(andi) {
    *ddest = *(id_src1->preg) & id_src2->simm;
    print_asm_template3(andi);
}

static inline def_EHelper(ori) {
    *ddest = *(id_src1->preg) | id_src2->simm;
    print_asm_template3(ori);
}

static inline def_EHelper(xori) {
    *ddest = *(id_src1->preg) ^ id_src2->simm;
    print_asm_template3(xori);
}

static inline def_EHelper(add) {
    rtl_add(s, ddest, dsrc1, dsrc2);
    print_asm_template3(add);
}

static inline def_EHelper(sub) {
    rtl_sub(s, ddest, dsrc1, dsrc2);
    print_asm_template3(sub);
}

static inline def_EHelper(slt) {
    *ddest = ((int)*dsrc1 - (int)*dsrc2) < 0 ? 1 : 0;
    print_asm_template3(slt);
}

static inline def_EHelper(sltu) {
    *ddest = (int)(*dsrc1 - *dsrc2) < 0 ? 1 : 0; 
    print_asm_template3(sltu);
}