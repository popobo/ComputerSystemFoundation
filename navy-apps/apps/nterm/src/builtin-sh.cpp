#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

#define MAX_ARGS (32)

static void sh_handle_cmd(const char *cmd) {
    if (cmd == NULL) {
        return;
    }

    int32_t cmd_len = strlen(cmd);
    if (cmd_len <= 1) {
        return;
    }
    char strip_cmd[cmd_len + 1];
    strcpy(strip_cmd, cmd);
    strip_cmd[cmd_len - 1] = '\0';
    
    char *args[MAX_ARGS] = {};
    char *token = NULL;
    char *saveptr = NULL;
    const char *delimit = " ";
    token = strtok_r(strip_cmd, delimit, &saveptr);
    args[0] = token;

    size_t args_index = 1;
    while ((token = strtok_r(NULL, delimit, &saveptr)) != NULL) {
        args[args_index++] = token;
    }
    
    execvp(args[0], args);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();
  
  setenv("PATH", "/bin:/usr/bin", 0);

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
