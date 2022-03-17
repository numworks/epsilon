
SECTIONS {
  .signed_payload_prefix ORIGIN(FLASH) : {
    FILL(0xFF);
    BYTE(0xFF)
    . = ORIGIN(FLASH) + SIGNED_PAYLOAD_LENGTH;
  } >FLASH

  .kernel_header : {
    KEEP(*(.kernel_header))
  } >FLASH

  .slot_info : {
    *(.slot_info*)
  } >SRAM

  .isr_vector_table ORIGIN(SRAM) + 512 : AT(ORIGIN(FLASH) + SIZEOF(.signed_payload_prefix) + SIZEOF(.kernel_header)) {
    /* When booting, the STM32F412 fetches the content of address 0x0, and
     * extracts from it various key infos: the initial value of the PC register
     * (program counter), the initial value of the stack pointer, and various
     * entry points to interrupt service routines. This data is called the ISR
     * vector table.
     *
     * Note that address 0x0 is always an alias. It points to the beginning of
     * Flash, SRAM, or integrated bootloader depending on the boot mode chosen.
     * (This mode is chosen by setting the BOOTn pins on the chip).
     *
     * We're generating the ISR vector table in code because it's very
     * convenient: using function pointers, we can easily point to the service
     * routine for each interrupt. */
    _isr_vector_table_start_flash = LOADADDR(.isr_vector_table);
    _isr_vector_table_start_ram = .;
    KEEP(*(.isr_vector_table))
    _isr_vector_table_end_ram = .;
  } >SRAM

  .exam_mode_buffer ORIGIN(FLASH) + SIZEOF(.signed_payload_prefix) + SIZEOF(.kernel_header) + SIZEOF(.isr_vector_table) : {
    . = ALIGN(4K);
    _exam_mode_buffer_start = .;
    KEEP(*(.exam_mode_buffer))
    /* Note: We don't increment "." here, we set it. */
    . = . + FIRST_FLASH_SECTOR_SIZE;
    _exam_mode_buffer_end = .;
  } >FLASH

  /* External flash memory */
  .userland_header : {
    . = ORIGIN(FLASH) + USERLAND_OFFSET;
    KEEP(*(.userland_header));
  } > FLASH

  .text : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >FLASH

  .rodata : {
    *(.rodata)
    *(.rodata.*)
  } >FLASH

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
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
  } >SRAM AT> FLASH

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
    . = (ORIGIN(SRAM) + LENGTH(SRAM) - STACK_SIZE);
    _heap_end = .;
  } >SRAM

  .stack : {
    . = ALIGN(8);
    _stack_end = .;
    . += (STACK_SIZE - 8);
    . = ALIGN(8);
    _stack_start = .;
  } >SRAM

  /DISCARD/ : {
    /* exidx and extab are needed for unwinding, which we don't use */
    *(.ARM.exidx*)
    *(.ARM.extab*)
  }
}
