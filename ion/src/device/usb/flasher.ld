/* Create a USB DFU firmware that runs from RAM.
 * Flashing using ST's ROMed DFU bootloader is reliable but very slow. To make
 * flashing faster, we can leverage ST's bootloader to copy a small "flasher" in
 * RAM, and run it from there.
 * Caution: ST's bootloader uses some RAM, so we want to stay off of that memory
 * region. Per AN2606, section 47, it's using 0x20003000 - 0x2003FFFF; We'll try
 * to play safe and avoid the first 32KB of RAM. */

MEMORY {
  RAM_BUFFER (rw) : ORIGIN = 0x20000000 + 32K, LENGTH = 256K - 32K
}

/* The stack is quite large, and should really be equal to Epsilon's. Indeed,
 * we're making the Calculator object live on the stack, and it's quite large
 * (about 4K just for this single object). Using a stack too small will result
 * in some memory being overwritten (for instance, vtables that live in the
 * .rodata section).  */

STACK_SIZE = 32K;

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
