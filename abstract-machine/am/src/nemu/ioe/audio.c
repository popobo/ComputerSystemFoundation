#include <am.h>
#include <nemu.h>
#include <klib.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)
#define AUDIO_AVAI_OFFSET    (AUDIO_ADDR + 0x18)
#define AUDIO_OPEN_AUDIO     (AUDIO_ADDR + 0x1c)
#define AUDIO_AVAILABLE_END  (AUDIO_ADDR + 0x20)

void __am_audio_init() {
    outb(AUDIO_INIT_ADDR, true);
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
    if (NULL == cfg) {
        return;
    }
    cfg->present = true;
    cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
    if (NULL == ctrl) {
        return;
    }
    outl(AUDIO_FREQ_ADDR, ctrl->freq);
    outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
    outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
    outb(AUDIO_OPEN_AUDIO, true);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
    stat->count = inl(AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
    if (NULL == ctl) {
        return;
    }
    int32_t offset = inl(AUDIO_AVAI_OFFSET);

    size_t len = ctl->buf.end - ctl->buf.start;
    int32_t available_end = inl(AUDIO_AVAILABLE_END);
    
    if (available_end < len) {
        memcpy((void *)AUDIO_SBUF_ADDR + offset, ctl->buf.start, available_end);
        outl(AUDIO_COUNT_ADDR, available_end);

        offset = inl(AUDIO_AVAI_OFFSET);

        memcpy((void *)AUDIO_SBUF_ADDR + offset, ctl->buf.start, len - available_end);
        outl(AUDIO_COUNT_ADDR, len - available_end);
    } else {
        memcpy((void *)AUDIO_SBUF_ADDR + offset, ctl->buf.start, len);
        outl(AUDIO_COUNT_ADDR, len);
    }
}
