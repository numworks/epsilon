USERLAND_ORIGIN = EPSILON_OFFSET + UserlandVirtualOrigin;

MEMORY {
  FLASH_USERLAND (rx) : ORIGIN = USERLAND_ORIGIN, LENGTH = UserlandLength
  SRAM (rw) : ORIGIN = UserlandSRAMOrigin, LENGTH = UserlandSRAMLength
}


SECTIONS {
  .userland_header ORIGIN(FLASH_USERLAND) : {
    KEEP(*(.userland_header))
  } >FLASH_USERLAND

  .isr_vector_table : {
    KEEP(*(.isr_vector_table))
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
    /* Slot information goes At the beginning of the RAM to be easily retrieved
     * from DFU request.*/
    *(.slot_info*)
    *(.data)
    *(.data.*)
    _data_section_end_ram = .;
    _userland_flash_end = LOADADDR(.data) + SIZEOF(.data);
  } >SRAM AT> FLASH_USERLAND

  .signature : {
    FILL(0xFF);
    /* We write the last byte of the signature with a default value to make
     * sure the userland size takes into account the signature. */
    . += SignatureLength - 1;
    BYTE(0xFF)
  } >FLASH_USERLAND

  .external_apps : {
    . = ALIGN(64K);
    _external_apps_flash_start = .;
    . = (ORIGIN(FLASH_USERLAND) + LENGTH(FLASH_USERLAND));
    _external_apps_flash_end = .;
  } >FLASH_USERLAND

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
    /* The # char is used on symbols that need to be to be linked at the end of
     * the bss section */
    *(.bss.[^$]*)
    /* The compiler may choose to allocate uninitialized global variables as
     * COMMON blocks. This can be disabled with -fno-common if needed. */
    *(COMMON)
    _external_apps_RAM_start = .;
    KEEP(*(.bss.$app_buffer))
    KEEP(*(.bss.$poincare_pool))
    _bss_section_end_ram = .;
    *(.bss.*)
    _bss_section_assert_end_ram = .;
    ASSERT (( _bss_section_end_ram == _bss_section_assert_end_ram), ".bss.$app_buffer section should be the last symbol of bss to be included in the external apps RAM range.");
  } >SRAM

  .heap : {
    _heap_start = .;
    /* Note: We don't increment "." here, we set it. */
    . = (ORIGIN(SRAM) + LENGTH(SRAM) - UserlandStackLength);
    _external_apps_RAM_end = .;
    _heap_end = .;
  } >SRAM

  .process_stack : {
    . = ALIGN(8);
    _process_stack_end = .;
    . += (UserlandStackLength - 8);
    . = ALIGN(8);
    _process_stack_start = .;
  } >SRAM

  /DISCARD/ : {
    /* exidx and extab are needed for unwinding, which we don't use */
    *(.ARM.exidx*)
    *(.ARM.extab*)
  }
}
