INCLUDE config_flash.ld

MEMORY {
  BENCH (rx) : ORIGIN = INTERNAL_FLASH_ORIGIN, LENGTH = INTERNAL_FLASH_LENGTH
  SRAM (rw) : ORIGIN = SRAM_ORIGIN, LENGTH = SRAM_LENGTH
}

STACK_SIZE = 32K;

SECTIONS {
  .isr_vector_table ORIGIN(BENCH) : {
    _isr_start = .;
    KEEP(*(.isr_vector_table))
  } >BENCH

  .text : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >BENCH

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP(*(.init_array))
    _init_array_end = .;
  } >BENCH

  .rodata : {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata.*)
  } >BENCH

  .data ORIGIN(SRAM) : {
    . = ALIGN(4);
    *(.data)
    *(.data.*)
  } >SRAM AT >BENCH

  .bss : {
    . = ALIGN(4);
    *(.bss)
    *(.bss.*)
    *(COMMON)
  } >SRAM

  .stack : {
    . = ALIGN(8);
    _stack_end = .;
    . += (STACK_SIZE - 8);
    . = ALIGN(8);
    _stack_start = .;
  } >SRAM
}
