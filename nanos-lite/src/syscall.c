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
        // Log("SYS_yield!");
        c->GPRx = 0;
        yield();
        break;
    case SYS_exit:
        // Log("SYS_exit!");
        halt(a[1]);
        break;
    case SYS_write:
        Log("SYS_write, a[1]:%d, a[3]:%d", a[1], a[3]);
        if (NULL == (void *)a[2] || 0 == a[3]) {
            c->GPRx = -1;
            break;
        }
        if (1 == a[1] || 2 == a[1]) {
            char *buf = (char *)a[2];
            for (int32_t i = 0; i < a[3]; ++i) {
                putch(buf[i]);
            }
        }
        c->GPRx = a[3];
        break;
    case -1:
        Log("ignore unhandled syscall ID -1");
        break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
