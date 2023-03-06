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

static inline def_EHelper(csrrw_satp) {
    if (id_dest->reg != 0) {
        *ddest = cpu.satp;
    }
    cpu.satp = *dsrc1;
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

static inline def_EHelper(csrrs_satp) {
    *ddest = cpu.satp;
    if (id_src1->reg != 0) {
        cpu.satp &= (~*dsrc1);
    }
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