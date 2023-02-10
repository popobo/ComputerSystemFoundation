#include <common.h>

#ifdef HAS_IOE

#include <device/map.h>
#include <SDL2/SDL.h>
#include <monitor/log.h>

#define AUDIO_PORT 0x200 // Note that this is not the standard
#define AUDIO_MMIO 0xa1000200
#define STREAM_BUF 0xa0800000
#define STREAM_BUF_MAX_SIZE 65536
#define AUDIO_BLOCK_LEN 64
#define ADB_Q_L 1024 //65536 / 4096

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  reg_available_offset,
  reg_open_audio,
  reg_available_end,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;
static int32_t freq = 0;
static int32_t channels = 0;
static int32_t samples = 0;

struct audio_data_block {
    uint8_t *start;
    uint8_t *end;
    uint32_t bytes_used;
};

static int32_t queue_front = -1;
static int32_t queue_rear = -1;
static struct audio_data_block adb_queue[ADB_Q_L] = {};

static inline void init_audio_data_block() {    
    for (int i = 0; i < ADB_Q_L; ++i) {
        adb_queue[i].start = sbuf + AUDIO_BLOCK_LEN * i;
        adb_queue[i].end = adb_queue[i].start + AUDIO_BLOCK_LEN;
        adb_queue[i].bytes_used = 0;
    }
}

static inline bool is_adb_queue_full() {
    
    if ((queue_front == queue_rear + 1) || (queue_front == 0 && queue_rear == ADB_Q_L - 1)) {
        return true;
    }
    return false;
}

static inline bool is_adb_queue_empty() {
    if (queue_front == -1) {
        return true;
    }
    return false;
}

static inline void push_adb(int32_t data_len) {
    if (is_adb_queue_full()) {
        return;
    }

    if (-1 == queue_front) queue_front = 0;
    queue_rear = (queue_rear + 1) % ADB_Q_L;
    adb_queue[queue_rear].bytes_used = data_len;
}

static int32_t pop_data_len = 0;
static inline int32_t pop_adb() {
    if (is_adb_queue_empty()) {
        return -1;
    }

    int32_t result = queue_front;
    if (queue_front == queue_rear) {
        queue_front = -1;
        queue_rear = -1;
    } else {
        queue_front = (queue_front + 1) % ADB_Q_L;
    }
    pop_data_len += adb_queue[result].bytes_used;

    return result;
}

static inline int32_t audio_buffer_used() {
    int bytes_used = 0;
    for (int i = 0; i < ADB_Q_L; ++i) {
        bytes_used += adb_queue[i].bytes_used;
    }
    return bytes_used;
}

static inline void audio_play(void *userdata, uint8_t *stream, int len) {
    int32_t adb = 0;
    int32_t write_len = 0;

    while((is_adb_queue_empty() != true) && len > 0) {
        adb = pop_adb();
        write_len =  adb_queue[adb].bytes_used <= len ? adb_queue[adb].bytes_used : len;
        adb_queue[adb].bytes_used = 0;
        SDL_memcpy(stream, adb_queue[adb].start, write_len);
        stream += write_len;
        len -= write_len;
    }
    
    if (len > 0) {
        SDL_memset(stream, 0, len);
    }
}

static inline void init_sdl_audio() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
}

static inline void open_sdl_audio() {
    SDL_AudioSpec desired_spec = {};
    desired_spec.freq = freq;
    desired_spec.channels = channels;
    desired_spec.samples = samples;
    desired_spec.callback = audio_play;
    desired_spec.format = AUDIO_S16SYS;
    desired_spec.userdata = NULL;

    if (SDL_OpenAudio(&desired_spec, NULL) < 0) {
        Log("failed to SDL_OpenAudio");
        return;
    }

    SDL_PauseAudio(0);
}


static void audio_io_handler(uint32_t offset, int len, bool is_write) {
    switch (offset >> 2)
    {
    case reg_freq:
        freq = audio_base[reg_freq];
        break;
    case reg_channels:
        channels = audio_base[reg_channels];
        break;
    case reg_samples:
        samples = audio_base[reg_samples];
        break;
    case reg_init:
        init_sdl_audio();
        break;
    case reg_count:
        if (is_write) {
            int count = audio_base[reg_count];
            int push_len = 0;
            while(count > 0) {
                push_len = count > AUDIO_BLOCK_LEN ? AUDIO_BLOCK_LEN : count;
                push_adb(push_len);
                count -= push_len;
            }
        } else {
            audio_base[reg_count] = audio_buffer_used();
        }
        break;
    case reg_available_offset:
        audio_base[reg_available_offset] = (int32_t)(adb_queue[(queue_rear + 1) % ADB_Q_L].start - sbuf);
        break;
    case reg_open_audio:
        open_sdl_audio();
        break;
    case reg_available_end:
        audio_base[reg_available_end] = sbuf + STREAM_BUF_MAX_SIZE - adb_queue[(queue_rear + 1) % ADB_Q_L].start;
        break;
    default:
        break;
    }
}


void init_audio() {
    uint32_t space_size = sizeof(uint32_t) * nr_reg;
    audio_base = (void *)new_space(space_size);
    add_pio_map("audio", AUDIO_PORT, (void *)audio_base, space_size, audio_io_handler);
    add_mmio_map("audio", AUDIO_MMIO, (void *)audio_base, space_size, audio_io_handler);
    
    audio_base[reg_sbuf_size] = STREAM_BUF_MAX_SIZE;

    sbuf = (void *)new_space(STREAM_BUF_MAX_SIZE);
    add_mmio_map("audio-sbuf", STREAM_BUF, (void *)sbuf, STREAM_BUF_MAX_SIZE, NULL);
    
    init_audio_data_block();
}
#endif	/* HAS_IOE */
