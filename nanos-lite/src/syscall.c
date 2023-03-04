#include <common.h>
#include "fs.h"
#include "syscall.h"
#include <proc.h>

// typedef struct {} PCB;

extern PCB *current;
void switch_boot_pcb();
void naive_uload(PCB *pcb, const char *filename);
int32_t context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit: {
            char *argv[] = { "/bin/nterm", NULL };
            context_uload(current, "/bin/nterm", argv, NULL);
            switch_boot_pcb();
            yield();
            break;
        }
    case SYS_write:
        if (NULL == (void *)a[2]) {
            c->GPRx = -1;
            assert(false);
            break;
        }
        c->GPRx = fs_write(a[1], (void *)a[2], a[3]);
        c->epc += 4;
        break;
    case SYS_open:
        c->GPRx = fs_open((const char *)a[1], a[2], a[3]);
        c->epc += 4;
        break;
    case SYS_read:
        c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
        c->epc += 4;
        break;
    case SYS_lseek:
        c->GPRx = fs_lseek(a[1], a[2], a[3]);
        c->epc += 4;
        break;
    case SYS_close:
        c->GPRx = fs_close(a[1]);
        c->epc += 4;
        break;
    case SYS_brk:
        c->GPRx = 0;
        c->epc += 4;
        break;
    case SYS_gettimeofday: {
        long *tv = (long *)a[1];
        if (NULL == tv) {
            c->GPRx = -1;
            assert(false);
            break;
        }
        long us = io_read(AM_TIMER_UPTIME).us;
        
        tv[0] = us / (1000 * 1000);
        tv[1] = us % (1000 * 1000);
        c->GPRx = 0;
        c->epc += 4;
        break;
    }
    case SYS_execve: {
            int32_t ret = context_uload(current, (const char *)a[1], (char * const*)a[2], (char * const*)a[3]);
            if (ret < 0) {
                c->GPRx = -2;
                break;
            }
            switch_boot_pcb();
            yield();
            break;
        }
    case -1:
        Log("ignore unhandled syscall ID -1");
        break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
