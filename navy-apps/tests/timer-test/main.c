#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <sys/time.h>

uint32_t NDL_GetTicks();

int main() {
    struct timeval cur_tv = {};

    uint32_t last_ms = 0;
    while(1) {
        uint32_t cur_ms = NDL_GetTicks();
        if (cur_ms - last_ms > 500) {
            last_ms = cur_ms;
            printf("cur_ms: %d\n", (int32_t)(cur_ms));
        }
    }

    return 0;
}
