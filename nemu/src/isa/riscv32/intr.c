#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc) {
    /* TODO: Trigger an interrupt/exception with ``NO''.
    * That is, use ``NO'' to index the IDT.
    */
    cpu.sepc = s->seq_pc;
    cpu.scause = NO;
    s->jmp_pc = epc;
    s->is_jmp = true;
}

void query_intr(DecodeExecState *s) {
}
