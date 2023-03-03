#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
    volatile int32_t *temp = (int32_t *)malloc(sizeof(int32_t) * 8);
    *temp = 1;
    printf("_impure_ptr:%x\n", _impure_ptr);
    printf("temp:%x\n", (int32_t)temp);
    printf("*temp:%d\n", (int32_t)*temp);
  return _syscall_(SYS_yield, 0, 0, 0);
}
