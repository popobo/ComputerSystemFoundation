#include <am.h>
#include <nemu.h>
#include <riscv32.h>
#include <klib.h>

static uint64_t timer_init_time = 0;

void __am_timer_init() {
    uint32_t lower = inl(RTC_ADDR);
    uint32_t higher = inl(RTC_ADDR + sizeof(uint32_t));
    uint64_t us = (uint64_t)higher * 1000000;
    us += lower;
    timer_init_time = us;
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
    if (NULL == uptime) {
        return;
    }
    uint32_t lower = inl(RTC_ADDR);
    uint32_t higher = inl(RTC_ADDR + sizeof(uint32_t));
    uint64_t us = (uint64_t)higher * 1000000;
    us += lower;
    uptime->us = us - timer_init_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
