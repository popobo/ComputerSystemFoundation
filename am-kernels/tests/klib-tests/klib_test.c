#include "check_function.h"

void test_memset() {
    int l, r;
    for (l = 0; l < N; ++l) {
        for (r = l + 1; r <= N; ++r) {
            reset(global_data);
            uint8_t val = (l + r) / 2;
            memset(global_data + l, val, r - l);
            check_seq(0, l, 1, global_data);
            check_eq(l, r, val, global_data);
            check_seq(r, N, r + 1, global_data);
        }
    }
}

void test_memcpy() {
    uint8_t data[N];
    int l, r;

    for (l = 0; l < N; ++l) {
        for (r = l + 1; r <= N; ++r) {
            reset(global_data);
            uint8_t val = (l + r) / 2;
            memset(global_data + l, val, r - l);
            memcpy(data + l, global_data + l, r - l);
            check_eq(l, r, val, data);
        }
    }
}

void test_strlen() {
    int i = 0;
    int str_len = 0;

    for (i = 0; i < N; ++i) {
        reset_str(N, global_str, 'a');
        set_str_end(i, global_str);
        str_len = strlen(global_str);
        assert(str_len == i);
    }
}

void test_memcmp() {
    uint8_t data[N];
    int l, r;
    int cmp_ret = 0;

    for (l = 0; l < N; ++l) {
        for (r = l + 1; r <= N; ++r) {
            reset(global_data);
            uint8_t val = (l + r) / 2;
            memset(global_data + l, val, r - l);
            memcpy(data + l, global_data + l, r - l);
            cmp_ret = memcmp(data + l, global_data + l, r - l);
            assert(cmp_ret == 0);
            check_eq(l, r, val, data);
        }
    }
}

int main() {

    test_memset();
    test_memcpy();
    test_strlen();
    test_memcmp();

    return 0;
}