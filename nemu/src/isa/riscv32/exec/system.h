static inline def_rtl(cssrw, rtlreg_t *csr) {
    rtlreg_t initial_r1 = *dsrc1;
    if (id_dest->reg != 0) {
        *ddest = *csr;
    }
    *csr = initial_r1;
}

static inline def_rtl(cssrs, rtlreg_t *csr) {
    rtlreg_t initial_r1 = *dsrc1;
    *ddest = *csr;
    if (id_src1->reg != 0) {
        *csr &= (~initial_r1);
    }
}

static inline def_EHelper(csrrw_stvec) {
    rtl_cssrw(s, &cpu.stvec);
}

static inline def_EHelper(csrrw_sepc) {
    rtl_cssrw(s, &cpu.sepc);
}

static inline def_EHelper(csrrw_scause) {
    rtl_cssrw(s, &cpu.scause);
}

static inline def_EHelper(csrrw_sstatus) {
    rtl_cssrw(s, &cpu.sstatus);
}

static inline def_EHelper(csrrw_satp) {
    rtl_cssrw(s, &cpu.satp);
}

// csrrw rd, csr, rs1
// csrrw sp, mscratch, sp -> exchange sp and mascratch
static inline def_EHelper(csrrw_mscratch) {
    rtl_cssrw(s, &cpu.mscratch);
}

static inline def_EHelper(csrrs_stvec) {
    rtl_cssrs(s, &cpu.stvec);
}

static inline def_EHelper(csrrs_sepc) {
    rtl_cssrs(s, &cpu.sepc);
}

static inline def_EHelper(csrrs_scause) {
    rtl_cssrs(s, &cpu.scause);
}

static inline def_EHelper(csrrs_sstatus) {
    rtl_cssrs(s, &cpu.sstatus);
}

static inline def_EHelper(csrrs_satp) {
    rtl_cssrs(s, &cpu.satp);
}

static inline def_EHelper(csrrs_mscratch) {
    rtl_cssrs(s, &cpu.mscratch);
}

#define ENV_CALL_S (9)

static inline def_EHelper(ecall) {
    void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc);
    // 9 Environment call from S-mode
    raise_intr(s, ENV_CALL_S, cpu.stvec);
}

static inline def_EHelper(sret) {
    s->is_jmp = true;
    s->jmp_pc = cpu.sepc;
    
    uint32_t spie = cpu.sstatus & SPIE;
    // set sie spie
    cpu.sstatus = (spie == SPIE) ? (cpu.sstatus | SIE) : (cpu.sstatus & SIE);
    // set pie 1
    cpu.sstatus = cpu.sstatus | SPIE;
}