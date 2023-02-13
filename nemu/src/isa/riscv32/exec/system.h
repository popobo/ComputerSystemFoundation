static inline def_EHelper(csrrw_stvec) {
    *ddest = cpu.stvec;
    cpu.stvec = *dsrc1;
}

static inline def_EHelper(csrrw_sepc) {
    *ddest = cpu.sepc;
    cpu.sepc = *dsrc1;
}

static inline def_EHelper(csrrw_scause) {
    *ddest = cpu.scause;
    cpu.scause = *dsrc1;
}

static inline def_EHelper(ecall) {
    extern void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc);
    // 9 Environment call from S-mode
    raise_intr(s, 9, cpu.stvec);
}
