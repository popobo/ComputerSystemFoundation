#include <am.h>
#include <nemu.h>
#include <klib.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
static int initial_width = 0;
static int initial_height = 0;

void __am_gpu_init() {
    int i;
    AM_GPU_CONFIG_T info = io_read(AM_GPU_CONFIG);
    int w = info.width;
    int h = info.height;
    initial_width = info.width;
    initial_height = info.height;
    printf("w: %d, h: %d\n", info.width, info.height);
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    for (i = 0; i < w * h; ++i) {
        fb[i] = i;
    }
    outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t  width_height = inl(VGACTL_ADDR);
  
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = width_height >> 16, .height = (width_height << 16) >> 16,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
    int index = 0;
    for (int x = ctl->x; x < (ctl->x + ctl->w) && x < initial_width; ++x) {
        for (int y = ctl->y; y < (ctl->y + ctl->h) && y < initial_height; ++y) {
            if (NULL == ctl->pixels) {
                break;
            }
            size_t color_size = sizeof(uint32_t);
            outl(FB_ADDR + y * initial_width * color_size + x * color_size, ((uint32_t *)ctl->pixels)[index++]);//(((uint32_t**)ctl->pixels)[x][y]));
        }
    }
    if (ctl->sync) {
        outl(SYNC_ADDR, 1);
    }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
    status->ready = true;
}
