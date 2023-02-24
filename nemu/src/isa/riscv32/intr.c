#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc) {
    /* TODO: Trigger an interrupt/exception with ``NO''.
    * That is, use ``NO'' to index the IDT.
    */
    // For riscv32, we save the pc which points to current pc.
    cpu.sepc = cpu.pc;
    cpu.scause = NO;
    cpu.sstatus = 0xc0100;
    s->jmp_pc = epc;
    s->is_jmp = true;
}

void query_intr(DecodeExecState *s) {
}
