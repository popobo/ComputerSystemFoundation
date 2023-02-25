#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
    
    int32_t argc = args[0];
    printf("args:%x, argc:%d\n", args, argc);
    char **argv = (char **)((uintptr_t)args + sizeof(uintptr_t));
    char **envp = (char **)((uintptr_t)args + (argc + 2) * sizeof(uintptr_t));
    for (int i = 0; i < argc; ++i) {
        printf("argc[i]:%s\n", argv[i]);
    }

    int i = 0;
    while (envp[i] != NULL) {
        printf("envp[i]:%s\n", envp[i]);
        i++;
    }

    char *empty[] =  {NULL };
    environ = empty;
    exit(main(argc, argv, envp));
    assert(0);
}
