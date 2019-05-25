OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0x80000000;
  .text.init : { *(.text.boot) }
  . = ALIGN(0x20000);
  .text : { *(.text) }
  . = ALIGN(0x20000);
  .data : { *(.data) }
  .bss : { *(.bss) }
  _end = .;
}

