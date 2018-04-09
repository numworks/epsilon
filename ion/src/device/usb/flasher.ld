/* Create a flash bridge.
 * Load it at address 0x20004000 and execute it from there
 * TODO: Explain the 16K offset (ST's DFU ROMed bootloader)
 */

MEMORY {
  RAM_BUFFER (rw) : ORIGIN = 0x20000000 + 16K, LENGTH = 256K - 16K
}

STACK_SIZE = 4K;

SECTIONS {
  .isr_vector_table ORIGIN(RAM_BUFFER) : {
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
    . = ALIGN(4);
    *(.data)
    *(.data.*)
  } >RAM_BUFFER

  .bss : {
    . = ALIGN(4);
    _bss_section_start_ram = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    _bss_section_end_ram = .;
  } >RAM_BUFFER

  .stack : {
    . = ALIGN(8);
    _stack_end = .;
    . += (STACK_SIZE - 8);
    . = ALIGN(8);
    _stack_start = .;
  } >RAM_BUFFER

  .phony : {
    /* We won't do dynamic memory allocation */
    _heap_start = .;
    _heap_end = .;
    /* Effectively bypass copying .data to RAM */
    _data_section_start_flash = .;
    _data_section_start_ram = .;
    _data_section_end_ram = .;
  } >RAM_BUFFER
}
