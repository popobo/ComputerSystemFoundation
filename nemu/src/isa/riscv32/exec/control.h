static inline def_EHelper(jal) {
    // the last two lines are not executed in this format, why
    // *ddest = s->seq_pc + 4;
    // s->is_jmp = true;
    // s->jmp_pc = s->seq_pc + id_src1->simm;
    s->is_jmp = true;
    // seq_pc is 4 bytes ahead cpu.pc
    s->jmp_pc = cpu.pc + id_src1->simm;
    *ddest = cpu.pc + sizeof(cpu.pc);
    print_asm_template2(jal);
}

static inline def_EHelper(jalr) {
    *ddest = cpu.pc + sizeof(cpu.pc);
    s->is_jmp = true;
    s->jmp_pc = (*dsrc1 + id_src2->simm) & ~1;
    print_asm_template3(jalr);
}

static inline def_EHelper(beq) {
    if (*dsrc1 == *dsrc2) {
        rtl_brc(s, id_dest->simm);
        print_asm_template3(beq);
    }
}

static inline def_EHelper(bne) {
    if (*dsrc1 != *dsrc2) {
        rtl_brc(s, id_dest->simm);
        print_asm_template3(bne);
    }
}

static inline def_EHelper(blt) {
    if ((int)*dsrc1 < (int)*dsrc2) {
        rtl_brc(s, id_dest->simm);
        print_asm_template3(blt);
    }
}

static inline def_EHelper(bltu) {
    if (*dsrc1 < *dsrc2) {
        rtl_brc(s, id_dest->simm);
        print_asm_template3(bltu);
    }
}

static inline def_EHelper(bge) {
    if ((int)*dsrc1 >= (int)*dsrc2) {
        rtl_brc(s, id_dest->simm);
        print_asm_template3(bge);
    }
}

static inline def_EHelper(bgeu) {
    if (*dsrc1 >= *dsrc2) {
        rtl_brc(s, id_dest->simm);
        print_asm_template3(bgeu);
    }
}
