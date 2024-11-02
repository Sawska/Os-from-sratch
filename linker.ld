OUTPUT_FORMAT("binary")
ENTRY(_start)
SECTIONS
{
  . = 0x1000;                  /* Kernel load address */
  .text :
  {
    *(.text)
  }
  .data :
  {
    *(.data)
  }
  .bss :
  {
    *(.bss)
  }
}
