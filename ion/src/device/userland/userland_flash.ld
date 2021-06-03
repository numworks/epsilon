/* Linker script
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements (for example, we want the code to be
 * written at a specific address (in Flash ROM) and the data at another. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">INTERNAL_FLASH" to ask for a given section to
 * be stored in Flash. */

USERLAND_ORIGIN = USERLAND_VIRTUAL_ORIGIN + EPSILON_OFFSET;
EXTERNAL_APPS_ORIGIN = EXTERNAL_APPS_VIRTUAL_ORIGIN + EPSILON_OFFSET;

MEMORY {
  FLASH_USERLAND (rx) : ORIGIN = USERLAND_ORIGIN, LENGTH = USERLAND_LENGTH
  FLASH_EXTERNAL_APPS (rx) : ORIGIN = EXTERNAL_APPS_ORIGIN, LENGTH = EXTERNAL_APPS_LENGTH
  SRAM (rw) : ORIGIN = USERLAND_SRAM_ORIGIN, LENGTH = USERLAND_SRAM_LENGTH
}


SECTIONS {
  .isr_vector_table ORIGIN(FLASH_USERLAND) : {
    KEEP(*(.isr_vector_table))
  } >FLASH_USERLAND

  .header : {
    KEEP(*(.header))
  } >FLASH_USERLAND

  .text : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >FLASH_USERLAND

  .rodata : {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata.*)
  } >FLASH_USERLAND

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >FLASH_USERLAND

  .external_apps : {
    _external_apps_start = .;
    . += EXTERNAL_APPS_LENGTH;
    _external_apps_end = .;
  } >FLASH_EXTERNAL_APPS

  .data : {
    /* The data section is written to Flash but linked as if it were in RAM.
     *
     * This is required because its initial value matters (so it has to be in
     * persistant memory in the first place), but it is a R/W area of memory
     * so it will have to live in RAM upon execution (in linker lingo, that
     * translates to the data section having a LMA in Flash and a VMA in RAM).
     *
     * This means we'll have to copy it from Flash to RAM on initialization.
     * To do this, we'll need to know the source location of the data section
     * (in Flash), the target location (in RAM), and the size of the section.
     * That's why we're defining three symbols that we'll use in the initial-
     * -ization routine. */
    . = ALIGN(4);
    _data_section_start_flash = LOADADDR(.data);
    _data_section_start_ram = .;
    *(.data)
    *(.data.*)
    _data_section_end_ram = .;
  } >SRAM AT> FLASH_USERLAND

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
  } >SRAM

  .heap : {
    _heap_start = .;
    /* Note: We don't increment "." here, we set it. */
    . = (ORIGIN(SRAM) + LENGTH(SRAM) - USERLAND_STACK_LENGTH);
    _heap_end = .;
  } >SRAM

  .process_stack : {
    . = ALIGN(8);
    _process_stack_end = .;
    . += (USERLAND_STACK_LENGTH - 8);
    . = ALIGN(8);
    _process_stack_start = .;
  } >SRAM

  /DISCARD/ : {
    /* exidx and extab are needed for unwinding, which we don't use */
    *(.ARM.exidx*)
    *(.ARM.extab*)
  }
}
