#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
    if (NULL == ref_r) {
        return false;
    }
    
    for (int i = 0; i < sizeof(ref_r->gpr) / sizeof(ref_r->gpr[0]); ++i) {
        if (ref_r->gpr[i]._32 != cpu.gpr[i]._32) {
            return false;
        }
    }

    return true;
}

void isa_difftest_attach() {
}
