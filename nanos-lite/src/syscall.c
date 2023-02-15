#include <common.h>
#include "syscall.h"
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield:
        c->GPRx = 0;
        yield();
        break;
    case SYS_exit:
        halt(a[1]);
        break;
    case -1:
        //Log("ignore unhandled syscall ID -1");
        break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
