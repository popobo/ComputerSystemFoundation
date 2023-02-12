ENTRY(_start)

/*riscv32 __pmem_start is 0x80000000*/
SECTIONS {
  . = _pmem_start + 0x100000; /* 0x80100000 */
  .text : {
    *(entry)
    *(.text*)
  }
  etext = .;
  _etext = .;
  .rodata : {
    *(.rodata*)
  }
  .data : {
    *(.data)
  }
  edata = .;
  _data = .;
  .bss : {
	_bss_start = .;
    *(.bss*)
    *(.sbss*)
    *(.scommon)
  }
  _stack_top = ALIGN(0x1000);
  . = _stack_top + 0x8000; /* stack size 0x8000B */
  _stack_pointer = .;
  end = .;
  _end = .;
  _heap_start = ALIGN(0x1000);
}
