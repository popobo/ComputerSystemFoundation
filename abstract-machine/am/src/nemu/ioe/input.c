#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
    int current_key = inl(KBD_ADDR);
    kbd->keydown = (current_key & KEYDOWN_MASK) == KEYDOWN_MASK ? true : false;
    kbd->keycode = current_key & ~KEYDOWN_MASK;
}
