#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <sys/time.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

enum {
    KEY_NONE,
    KEY_WIDTH,
    KEY_HEIGHT,
};

static struct rule  {
    char *regex;
    int token_type;
} rules[] = {
    {"WIDTH:[0-9]+", KEY_WIDTH},
    {"HEIGHT:[0-9]+", KEY_HEIGHT},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {};

static inline void init_regex() {
    int i = 0;
    char error_msg[128] = {};
    int ret = 0;
    for (int i = 0; i < NR_REGEX; ++i) {
        ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret != 0) {
            regerror(ret, &re[i], error_msg, 128);
            printf("reg comp error: %s\n", error_msg);
            return;
        }
    }
}

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
    FILE *fp = fopen("/dev/events", "r+");
    
    return fread(buf, sizeof(char), len, fp);
}

void NDL_OpenCanvas(int *w, int *h) {
    
    char buf[1024] = {};
    FILE *fp = fopen("/proc/dispinfo", "r+");
    fread(buf, sizeof(char), sizeof(buf)/sizeof(buf[0]), fp);
    printf("%d, buf:%s\n", __LINE__, buf);
    int width = get_value_by_key(buf, "WIDTH");
    *w = width < 0 ? 0 : width;
    int height = get_value_by_key(buf, "HEIGHT");
    *h = height < 0 ? 0 : height;
    
    printf("width: %d, height:%d\n", *w, *h);
    
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
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
    init_regex();
}
