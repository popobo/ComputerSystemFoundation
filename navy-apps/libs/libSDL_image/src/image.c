#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
    int32_t fd = open(filename, O_RDONLY);
    int32_t filesize = lseek(fd, 0, SEEK_END);
    assert(filesize != -1);

    uint8_t* buffer = (uint8_t *)malloc(filesize);
    lseek(fd, 0, SEEK_SET);
    int32_t read_size = read(fd, buffer, filesize);
    assert(read_size == filesize);

    SDL_Surface *surface = STBIMG_LoadFromMemory(buffer, filesize);
    assert(surface != NULL);

    close(fd);
    free(buffer);

    return surface;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
