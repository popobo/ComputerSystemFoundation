#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>

static int32_t evtdev = -1;
static int32_t fbdev = -1;
static int32_t screen_w = 0, screen_h = 0;

static inline int32_t get_value_by_key(char *source, char *key) {
    if (NULL == source || NULL == source) {
        return -1;
    }
    
    size_t key_len = strlen(key);
    char *digit_value = ":([0-9]+)";
    size_t digit_value_len = strlen(digit_value);
    char reg_buf[key_len + digit_value_len + 1];
    strcpy(reg_buf, key);
    strcpy(reg_buf + key_len, digit_value);
    

    regex_t rt = {};
    int32_t ret = regcomp(&rt, reg_buf, REG_EXTENDED);
    if (ret != 0) {
        return -1;
    }

    size_t buf_index = 0;
    size_t source_len = strlen(source);
    char buf[source_len + 1];
    for (int32_t i = 0; i < source_len; ++i) {
        if (source[i] != ' ' && source[i] != '\n') {
            buf[buf_index++] = source[i];
        }
    }

    buf[buf_index] = '\0';
    
    const size_t max_matches = 1;
    const size_t max_groups = 2;
    regmatch_t group_array[max_groups]; 
    if (regexec(&rt, buf, max_groups, group_array, 0) != 0) {
        return -1;
    }

    if (((size_t) - 1) == group_array[max_groups - 1].rm_so) {
        // no more groups
        return -1;
    }
    
    size_t digit_buf_len = group_array[max_groups - 1].rm_eo - group_array[max_groups - 1].rm_so + 1 + 1;
    char digit_buf[digit_buf_len];
    strncpy(digit_buf, buf + group_array[max_groups - 1].rm_so, digit_buf_len - 1);
    digit_buf[digit_buf_len - 1] = '\0';
    int32_t result = atoi(digit_buf);

    return result;
}
uint32_t NDL_GetTicks() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint32_t result = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    if (tv.tv_usec % 1000 > 500) {
        result++;
    }
    return result;
}

int NDL_PollEvent(char *buf, int len) {
    int fd = open("/dev/events", O_RDONLY);

    return read(fd, buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
    
    char buf[256] = {};
    int fd = open("/proc/dispinfo", O_RDONLY);
    lseek(fd, 0, SEEK_SET);
    read(fd, buf, sizeof(buf)/sizeof(buf[0]));
    int width = get_value_by_key(buf, "WIDTH");
    int height = get_value_by_key(buf, "HEIGHT");
    
    if (*w == 0 && *h == 0) {
        *w = width < 0 ? 0 : width;
        *h = height < 0 ? 0 : height;
    }

    screen_w = width;
    screen_h = height;

    printf("width: %d, height:%d\n", screen_w, screen_h);
    
//   if (getenv("NWM_APP")) {
//     int fbctl = 4;
//     fbdev = 5;
//     screen_w = *w; screen_h = *h;
//     char buf[64];
//     int len = sprintf(buf, "%d %d", screen_w, screen_h);
//     // let NWM resize the window and create the frame buffer
//     write(fbctl, buf, len);
//     while (1) {
//       // 3 = evtdev
//       int nread = read(3, buf, sizeof(buf) - 1);
//       if (nread <= 0) continue;
//       buf[nread] = '\0';
//       if (strcmp(buf, "mmap ok") == 0) break;
//     }
//     close(fbctl);
//   }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
    int32_t fd = open("/dev/fb", O_RDWR);
    int32_t offset = 0;
    int32_t len = 0;
    int32_t pixel_size = sizeof(uint32_t);
    uint8_t *byte_pixels = (uint8_t *)pixels;

    for (int i = y; i < y + h + 1; ++i) {
        offset = (screen_w * i + x) * pixel_size;
        lseek(fd, offset, SEEK_SET);
        write(fd,
            byte_pixels + pixel_size * w * i, 
            w * pixel_size);
    }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}


int32_t NDL_GetScreenWidth() {
    return screen_w;
}

int32_t NDL_GetScreenHeight() {
    return screen_h;
}