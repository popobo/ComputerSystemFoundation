#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc) {
    /* TODO: Trigger an interrupt/exception with ``NO''.
    * That is, use ``NO'' to index the IDT.
    */
    // For riscv32, we save the pc which points to current pc.
    cpu.sepc = cpu.pc;
    cpu.scause = NO;
    s->jmp_pc = epc;
    s->is_jmp = true;

    uint32_t sie = cpu.sstatus & SIE;
    assert(sie == SIE || sie == 0);

    // set spie sie
    cpu.sstatus = (SIE == sie) ? (cpu.sstatus | SPIE) : (cpu.sstatus & ~SPIE);
    // set sie 0
    cpu.sstatus &= ~SIE;
}

#define IRQ_TIMER 0x80000005

void query_intr(DecodeExecState *s) {
    if (cpu.INTR && ((cpu.sstatus & SIE) == SIE)) {
        cpu.INTR = false;
        raise_intr(s, IRQ_TIMER, cpu.stvec);
        update_pc(s);
    }
}

