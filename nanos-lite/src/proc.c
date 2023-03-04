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
        // if (j % 1000000 == 0) {
            Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (uintptr_t)arg, j);
        // }
        j ++;
        yield();
    }
}

void init_proc() {
    char *argv[] = { "/bin/dummy", NULL };
    context_uload(&pcb[0], "/bin/dummy", argv, NULL);
    printf("init_proc pcb[1].as.ptr:%x, (uint32_t)pcb[1].cp:%x\n", pcb[1].as.ptr, (uint32_t)pcb[1].cp);
    switch_boot_pcb();
}

Context* schedule(Context *prev) {
    // printf("schedule pcb[0].as.ptr:%x, pcb[0].cp->pdir:%x\n", pcb[0].as.ptr, pcb[0].cp->pdir);
    // save the context pointer, what is prev
    
    current->cp = prev;
    // printf("schedule 1 current:%x, current->as.ptr:%x\n", (uint32_t)current, (uint32_t)current->as.ptr);
    current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

    // printf("schedule 2 current:%x, current->as.ptr:%x, current->cp:%x, &current->cp:%x\n", (uint32_t)current, (uint32_t)current->as.ptr, (uint32_t)current->cp, &(current->cp));
    // printf("schedule 3 pcb[1].as.ptr:%x, (uint32_t)pcb[1].cp:%x\n", (uint32_t)pcb[1].as.ptr, (uint32_t)pcb[1].cp);
    // then return the new context
    return current->cp;
}
