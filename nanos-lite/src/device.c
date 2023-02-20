#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
    char *char_buf = (char *)buf;
    for (int32_t i = 0; i < len; ++i) {
        putch(char_buf[i]);
    }
    return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
    AM_INPUT_KEYBRD_T key_t = io_read(AM_INPUT_KEYBRD);
    if (key_t.keycode == AM_KEY_NONE) {
        return 0;
    }

    if (NULL == buf) {
        return 0;
    }
    
    // should use snprintf ?
    return sprintf(buf,
            true == key_t.keydown ? "kd %s\n" : "ku %s\n", 
            keyname[key_t.keycode]);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
    int width = io_read(AM_GPU_CONFIG).width;
    int height = io_read(AM_GPU_CONFIG).height;

    return sprintf(
        buf, 
        "WIDTH:%d"
        "HEIGHT:%d",
        width, height
    );
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
    int32_t width = io_read(AM_GPU_CONFIG).width;
    
    int32_t x = (offset >> 2) % width;
    int32_t y = (offset >> 2) / width;

    io_write(AM_GPU_FBDRAW, x, y, (void *)buf, len >> 2, 1, true);

    return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
