INCLUDE config_flash.ld

CONFIG_OFFSET = FLASHER_OFFSET;
CONFIG_LENGTH = FLASHER_LENGTH;

INCLUDE config_ram.ld

SECTIONS {
  .isr_vector_table ORIGIN(RAM_BUFFER) : {
    _isr_start = .;
    KEEP(*(.isr_vector_table))
  } >RAM_BUFFER

  .text : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >RAM_BUFFER

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >RAM_BUFFER

  .rodata : {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata.*)
  } >RAM_BUFFER

  .data : {
    *(.data)
    *(.data.*)
  } >RAM_BUFFER
  ASSERT(SIZEOF(.data) == 0, "Flasher .data section is not empty")

  .bss : {
    *(.bss)
    *(.bss.*)
    *(COMMON)
  } >RAM_BUFFER
  ASSERT(SIZEOF(.bss) == 0, "Flasher .bss section is not empty")

  .stack : {
    . = ALIGN(8);
    _stack_end = .;
    . += (STACK_SIZE - 8);
    . = ALIGN(8);
    _stack_start = .;
  } >RAM_BUFFER
}
