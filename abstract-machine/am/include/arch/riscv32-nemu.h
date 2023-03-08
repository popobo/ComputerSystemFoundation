#ifndef ARCH_H__
#define ARCH_H__

struct Context {
    union {
        // address space info
        void *pdir;
        uintptr_t gpr[32];
    };
    
    uintptr_t cause;
    uintptr_t status;
    uintptr_t epc;
    uintptr_t np;
};

#define GPR1 gpr[17] // a7
#define GPR2 gpr[10] 
#define GPR3 gpr[11]
#define GPR4 gpr[12]
#define GPRx gpr[10]

#endif
