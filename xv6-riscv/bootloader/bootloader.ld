OUTPUT_ARCH( "riscv" )

/* Task: ensure the correct starting function. */
ENTRY( _entry )

SECTIONS
{
  /* Task: ensure the correct entry address based on where -kernel in QEMU jumps. */
  . = 0x80000000;

  /* Task lay out the sections as mentioned in the handout */
  .text :
  {
    *(.text)
    PROVIDE(ecode = 0x800009fa);
  }

  /* No .data section in bootloader binary */
  .data :
  {
    *(.data)
    PROVIDE(edata = .);
  }

  .rodata :
  {
    *(.rodata)
    PROVIDE(erodata = 0x80000b00);
  }

  .bss :
  {
    *(.bss)
    PROVIDE(ebss = 0x80008bc0);
  }

  PROVIDE(end = 0x80008bd8);
}
