#include <am.h>
#include <riscv32.h>
#include <klib.h>

#define ENV_CALL_S (9) // Environment call from S-mode
#define sys_call_num (c->gpr[17])

static Context* (*user_handler)(Event, Context*) = NULL;
void __am_asm_trap(void);
void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

Context* __am_irq_handle(Context *c) {
    __am_get_cur_as(c);
    if (user_handler) {
        Event ev = {0};
        switch (c->cause) {
            case ENV_CALL_S:
                if (-1 == sys_call_num) {
                    ev.event = EVENT_YIELD;  
                    break;
                }
                ev.event = EVENT_SYSCALL;
                break;
            default: ev.event = EVENT_ERROR; break;
        }

        c = user_handler(ev, c);
        assert(c != NULL);
    }
    __am_switch(c);
    return c;
}

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
    assert(kstack.end != NULL && kstack.start != NULL);

    Context * cp = (Context *)((uintptr_t)kstack.end - sizeof(Context));
    cp->epc = (uintptr_t)entry;
    cp->status = 0xc0100; //For DiffTest
    cp->cause = 0;
    for (int i = 0; i < sizeof(cp->gpr) / sizeof(cp->gpr[0]); ++i) {
        cp->gpr[i] = 0;
    }
    cp->gpr[10] = (uintptr_t)arg;
    cp->pdir = NULL;

    return cp;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
