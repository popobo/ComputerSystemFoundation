#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <sys/time.h>

int main() {
    struct timeval cur_tv = {};
    
    uint64_t total_us_cur = 0;
    uint64_t total_us_last = 0;
    while(1) {
        gettimeofday(&cur_tv, NULL);
        total_us_cur = cur_tv.tv_sec * 1000 * 1000 + cur_tv.tv_usec;
        if (total_us_cur - total_us_last > 500 * 1000) {
            printf("total_us_cur: %ld\n", (long)(total_us_cur));
            total_us_last = total_us_cur;
        }
    }

    return 0;
}
