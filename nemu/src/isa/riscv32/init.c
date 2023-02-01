#include <isa.h>
#include <memory/paddr.h>

// this is not consistent with uint8_t
// but it is ok since we do not access the array directly
static const uint32_t img [] = {
  0x00130293,  // addi t0,t1,0x001
  0x800002b7,  // lui t0,0x80000
  0x000012b7,  // lui t0,0x00001
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
