CROSS_COMPILE = riscv64-linux-gnu-
LNK_ADDR = 0x40000000
CFLAGS  += -fno-pic -march=rv32g -mabi=ilp32
LDFLAGS += -melf32lriscv --no-relax -Ttext-segment $(LNK_ADDR)
