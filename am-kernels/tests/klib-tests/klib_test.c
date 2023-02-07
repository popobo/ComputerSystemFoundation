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
        }
    }

    for (l = 0; l < N; ++l) {
        for (r = l + 1; r <= N; ++r) {
            reset(global_data);
            reset(data);
            uint8_t val = (l + r) / 2;
            memset(global_data + l, val, r - l);
            memcpy(data + l, global_data + l, r - l);
            data[(l + r) / 2] += 1;
            cmp_ret = memcmp(data + l, global_data + l, r - l);
            assert(cmp_ret > 0);
            if (data[(l + r) / 2] > 1) {
                data[(l + r) / 2] -= 2;
                cmp_ret = memcmp(data + l, global_data + l, r - l);
                assert(cmp_ret < 0);
            }
        }
    }
}

void test_strcmp(){
    char *str1 = "abcdef";
    char *str2 = "abcde";
    char *str3 = "abcdefg";
    char *str4 = "abcdef";
    char *str5 = "";

    int cmp_ret = 0;
    
    cmp_ret = strcmp(str1, str2);
    assert(cmp_ret > 0);

    cmp_ret = strcmp(str1, str3);
    assert(cmp_ret < 0);
    
    cmp_ret = strcmp(str1, str4);
    assert(cmp_ret == 0);

    cmp_ret = strcmp(str1, str5);
    assert(cmp_ret > 0);

    cmp_ret = strcmp(str5, str1);
    assert(cmp_ret < 0);
}

void test_strncmp() {
    char *str1 = "abcdef";
    char *str2 = "abcde";
    char *str3 = "abcdefg";
    char *str4 = "abcdef";
    char *str5 = "";
    char *str6 = "";

    int cmp_ret = 0;

    //max length 8
    cmp_ret = strncmp(str1, str2, 8);
    assert(cmp_ret > 0);

    cmp_ret = strncmp(str1, str3, 8);
    assert(cmp_ret < 0);
    
    cmp_ret = strncmp(str1, str4, 8);
    assert(cmp_ret == 0);

    cmp_ret = strncmp(str1, str5, 8);
    assert(cmp_ret > 0);

    cmp_ret = strncmp(str5, str1, 8);
    assert(cmp_ret < 0);

    cmp_ret = strncmp(str1, str2, strlen(str2));
    assert(cmp_ret == 0);

    cmp_ret = strncmp(str1, str3, strlen(str1));
    assert(cmp_ret == 0);

    cmp_ret = strncmp(str1, str5, strlen(str5));
    assert(cmp_ret == 0);

    cmp_ret = strncmp(str5 ,str6, strlen(str6));
    assert(cmp_ret == 0);
}


int main() {

    test_memset();
    test_memcpy();
    test_strlen();
    test_memcmp();
    test_strcmp();
    test_strncmp();

    return 0;
}