static inline def_EHelper(csrrw_stvec) {
    if (id_dest->reg != 0) {
        *ddest = cpu.stvec;
    }
    cpu.stvec = *dsrc1;
}

static inline def_EHelper(csrrw_sepc) {
    
    if (id_dest->reg != 0) {
        *ddest = cpu.sepc;
    }
    cpu.sepc = *dsrc1;
}

static inline def_EHelper(csrrw_scause) {
    if (id_dest->reg != 0) {
        *ddest = cpu.scause;
    }
    cpu.scause = *dsrc1;
}

static inline def_EHelper(csrrw_sstatus) {
    if (id_dest->reg != 0) {
        *ddest = cpu.sstatus;
    }
    cpu.sstatus = *dsrc1;
}

static inline def_EHelper(csrrs_stvec) {
    *ddest = cpu.stvec;
    if (id_src1->reg != 0) {
        cpu.stvec &= (~*dsrc1);
    }
}

static inline def_EHelper(csrrs_sepc) {
    *ddest = cpu.sepc;
    if (id_src1->reg != 0) {
        cpu.sepc &= (~*dsrc1);
    }
}

static inline def_EHelper(csrrs_scause) {
    *ddest = cpu.scause;
    if (id_src1->reg != 0) {
        cpu.scause &= (~*dsrc1);
    }
}

static inline def_EHelper(csrrs_sstatus) {
    *ddest = cpu.sstatus;
    if (id_src1->reg != 0) {
        cpu.sstatus &= (~*dsrc1);
    }
}

static inline def_EHelper(ecall) {
    extern void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc);
    // 9 Environment call from S-mode
    raise_intr(s, 9, cpu.stvec);
}

static inline def_EHelper(sret) {
    s->is_jmp = true;
    s->jmp_pc = cpu.sepc + sizeof(cpu.sepc);
}