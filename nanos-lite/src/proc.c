#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
// PCB of current process
PCB *current = NULL;

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
    int j = 1;
    while (1) {
        if (j % 1000000 == 0) {
            Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (uintptr_t)arg, j);
        }
        
        j ++;
        yield();
    }
}

void init_proc() {
    char *argv[] = { "/bin/dummy", NULL };
    context_uload(&pcb[0], "/bin/dummy", argv, NULL);
    context_uload(&pcb[1], "/bin/dummy", argv, NULL);
    switch_boot_pcb();
}

Context* schedule(Context *prev) {
    // save the context pointer, what is prev
    current->cp = prev;

    current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

    // then return the new context
    return current->cp;
}
