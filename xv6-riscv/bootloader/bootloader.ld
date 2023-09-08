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
    PROVIDE(ecode = 0x800009e6);
  }

  .data :
  {
    *(.data)
    PROVIDE(edata = 0x80000a08);
  }

  .rodata :
  {
    *(.rodata)
    PROVIDE(erodata = 0x80000b08);
  }

  .bss :
  {
    *(.bss)
    PROVIDE(ebss = 0x80008ba0);
  }

  PROVIDE(end = 0x80008bb8);
}
