#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
    
    int32_t argc = args[0];
    char **argv = (char **)((uintptr_t)args + sizeof(uintptr_t));
    char **envp = (char **)((uintptr_t)args + (argc + 2) * sizeof(uintptr_t));

    char *empty[] =  {NULL };
    environ = empty;
    exit(main(argc, argv, envp));
    assert(0);
}
