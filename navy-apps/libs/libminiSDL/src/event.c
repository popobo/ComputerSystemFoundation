#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
    assert(event != NULL);
    char buf[64];
    int event_len = NDL_PollEvent(buf, sizeof(buf));
    if (0 == event_len) {
        return 0;
    }

    //replace '\n' to do easier comparison
    buf[event_len - 1] = '\0';

    if (strncmp(buf, "kd", 2) == 0) {
        event->key.type = SDL_KEYDOWN;
    } else if (strncmp(buf, "ku", 2) == 0) {
        event->key.type = SDL_KEYUP;
    } else {
        return 0;
    }

    for (int i = 0; i < sizeof(keyname)/sizeof(keyname[0]); ++i) {
        if (strcmp(buf + 3, keyname[i]) == 0) {
            event->key.keysym.sym = i;
            break;
        }
    }

    return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
