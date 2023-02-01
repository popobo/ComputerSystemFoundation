#include <isa.h>
#include <memory/paddr.h>

// this is not consistent with uint8_t
// but it is ok since we do not access the array directly
static const uint32_t img [] = {
  0x40f752b3,  // sra reg7,reg14,reg15
  0x00f752b3,  // srl reg7,reg14,reg15
  0x40f702b3,  // sub reg7,reg14,reg15
  0x00f702b3,  // add reg7,reg14,reg15
  0xfff002b7,  // lui t0,0xfff00
  0xaaa2f413,  // andi s0,t0,0xfffffaaa
  0x0002a413,  // stli, s0,t0,0x000
  0x0002b413,  // stli, s0,t0,0x000
  0x00001297,  // auipc pc,0x00001
  0x0002a023,  // sw  zero,0(t0)
  0x0002a503,  // lw  a0,0(t0)
  0x0000006b,  // nemu_trap
};

static void restart() {
  /* Set the initial program counter. */
  cpu.pc = PMEM_BASE + IMAGE_START;

  /* The zero register is always 0. */
  cpu.gpr[0]._32 = 0;
}

void init_isa() {
  /* Load built-in image. */
  memcpy(guest_to_host(IMAGE_START), img, sizeof(img));

  /* Initialize this virtual computer system. */
  restart();
}
