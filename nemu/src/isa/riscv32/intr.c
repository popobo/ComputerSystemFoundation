#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc) {
    /* TODO: Trigger an interrupt/exception with ``NO''.
    * That is, use ``NO'' to index the IDT.
    */
    cpu.sepc = cpu.pc;
    cpu.scause = NO;
    cpu.pc = epc;
}

void query_intr(DecodeExecState *s) {
}
