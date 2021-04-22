/* Linker script TODO: update
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements (for example, we want the code to be
 * written at a specific address (in Flash ROM) and the data at another. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">INTERNAL_FLASH" to ask for a given section to
 * be stored in Flash. */

INFO_API_LEVEL = 0;

MEMORY {
  FLASH (rx) : ORIGIN = 0, LENGTH = 11*64K
  SRAM_DATA_BSS (rw) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS {
  .app_info ORIGIN(FLASH) : {
    KEEP(*(.app_info))
  } >FLASH

  .text : {
    . = ALIGN(4);
    _main_symbol = .;
    KEEP(*(*._Z8app_mainv));
    *(.text)
    *(.text.*)
  } >FLASH

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >FLASH

  .rodata : {
    _title_symbol = .;
    KEEP(*(*._ZL5title))
    _upper_title_symbol = .;
    KEEP(*(*._ZL10upperTitle))
    _icon_symbol = .;
    KEEP(*(*.compressedPixelData))
    *(.rodata)
    *(.rodata.*)
  } >FLASH

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
    _app_flash_end = LOADADDR(.data) + SIZEOF(.data);
  } >SRAM_DATA_BSS AT> FLASH

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
  } >SRAM_DATA_BSS

  /DISCARD/ : {
    /* exidx and extab are needed for unwinding, which we don't use */
    *(.ARM.exidx*)
    *(.ARM.extab*)
  }
}

