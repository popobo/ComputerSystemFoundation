#include "klib.h"

#define N 32
uint8_t global_data[N];
char global_str[N] = { 'a' };

static inline void reset(uint8_t * data) {
    int i = 0;
    for (i = 0; i < N; ++i) {
        data[i] = i + 1;
    }
}

// check that the numbers in [l, r) are val, val + 1 ...
static inline void check_seq(int l, int r, int val, uint8_t * data) {
    int i = 0;
    for (i = l; i < r; ++i) {
        assert(data[i] == val + i - l);
    }
}

// check whether the numbers in [l, r) are all val
static inline void check_eq(int l, int r, int val, uint8_t * data) {
    int i = 0;
    for (i = l; i < r; ++i) {
        assert(data[i] == val);
    }
}

static inline void reset_str(int len, char *str, char ch) {
    int i;
    for (i = 0; i < len; ++i) {
        str[i] = ch;
    }
}

static inline void set_str_end(int index, char * str) {
    str[index] = '\0';
}