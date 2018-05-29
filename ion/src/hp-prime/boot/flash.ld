/* Linker script
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">DRAM" to ask for a given section to
 * be stored in DRAM. */
MEMORY {
  SRAM (rwx) : ORIGIN = 0x00000000, LENGTH = 64K
  DRAM (rwx) : ORIGIN = 0x30000000, LENGTH = 32768K
}

STACK_SIZE = 32K;

SECTIONS {
  .text : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >DRAM

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >DRAM

  .rodata : {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata.*)
  } >DRAM

  .data : {
    . = ALIGN(4);
    *(.data)
    *(.data.*)
  } >DRAM

  .bss : {
    /* The bss section contains data for all uninitialized variables
     * So like the .data section, it will go in RAM, but unlike the data section
     * we don't care at all about an initial value.
     *
     * Before execution, crt0 will erase that section of memory though, so we'll
     * need pointers to the beginning and end of this section. */
    . = ALIGN(4);
    _bss_section_start_ram = .;
    *(.bss)
    *(.bss.*)
    /* The compiler may choose to allocate uninitialized global variables as
     * COMMON blocks. This can be disabled with -fno-common if needed. */
    *(COMMON)
    _bss_section_end_ram = .;
  } >DRAM

  .heap : {
    _heap_start = .;
    /* Note: We don't increment "." here, we set it. */
    . = (ORIGIN(DRAM) + LENGTH(DRAM) - STACK_SIZE);
    _heap_end = .;
  } >DRAM

  .stack : {
    . = ALIGN(8);
    _stack_end = .;
    . += (STACK_SIZE - 8);
    . = ALIGN(8);
    _stack_start = .;
  } >DRAM
}
