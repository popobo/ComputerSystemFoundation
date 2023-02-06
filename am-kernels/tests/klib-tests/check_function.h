#include "klib.h"

#define N 32
uint8_t data[N];

static inline void reset() {
    int i = 0;
    for (i = 0; i < N; ++i) {
        data[i] = i + 1;
    }
}

// check that the numbers in [l, r) are val, val + 1 ...
static inline void check_seq(int l, int r, int val) {
    int i = 0;
    for (i = l; i < r; ++i) {
        assert(data[i] == val + i - l);
    }
}

// check whether the numbers in [l, r) are all val
static inline void check_eq(int l, int r, int val) {
    int i = 0;
    for (i = l; i < r; ++i) {
        assert(data[i] == val);
    }
}