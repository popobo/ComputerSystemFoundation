#include <common.h>

void do_syscall(Context *c);
Context* schedule(Context *prev);

static Context* do_event(Event e, Context* c) {
    assert(c != NULL);
    switch (e.event) {
        case EVENT_SYSCALL:
            do_syscall(c);
        case EVENT_YIELD:
            c = schedule(c);
            break;
        default: panic("Unhandled event ID = %d", e.event);
    }

    return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
