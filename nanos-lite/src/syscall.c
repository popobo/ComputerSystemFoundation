#include <common.h>
#include "fs.h"
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
        // Log("SYS_write, a[1]:%d, a[3]:%d", a[1], a[3]);
        if (NULL == (void *)a[2]) {
            c->GPRx = -1;
            break;
        }
        c->GPRx = fs_write(a[1], (void *)a[2], a[3]);
        break;
    case SYS_open:
        c->GPRx = fs_open((const char *)a[1], a[2], a[3]);
        break;
    case SYS_read:
        c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
        break;
    case SYS_lseek:
        c->GPRx = fs_lseek(a[1], a[2], a[3]);
        break;
    case SYS_close:
        c->GPRx = fs_close(a[1]);
    case SYS_brk:
        c->GPRx = 0;
        break;
    case SYS_gettimeofday: {
        long *tv = (long *)a[1];
        if (NULL == tv) {
            c->GPRx = -1;
            break;
        }
        long us = io_read(AM_TIMER_UPTIME).us;
        
        tv[0] = us / (1000 * 1000);
        tv[1] = us % (1000 * 1000);
        c->GPRx = 0;
        break;
    }
    case -1:
        Log("ignore unhandled syscall ID -1");
        break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
