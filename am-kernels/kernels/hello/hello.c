#include <am.h>
#include <klib-macros.h>
#include <klib.h>

int main(const char *args) {
  const char *fmt =
    "Hello, AbstractMachine!\n"
    "mainargs = '%'.\n";

  for (const char *p = fmt; *p; p++) {
    (*p == '%') ? putstr(args) : putch(*p);
  }

    printf("hello %d\n", 11111);

    return 0;
}
