#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
    int32_t *temp = (int32_t *)malloc(sizeof(int32_t));
    *temp = 666;
    printf("*temp:%d\n", *temp);
    free(temp);
    return _syscall_(SYS_yield, 0, 0, 0);
}
