PERSITING_BYTES_ORIGIN = PERSITING_BYTES_VIRTUAL_ORIGIN + EPSILON_OFFSET;
KERNEL_ORIGIN = KERNEL_VIRTUAL_ORIGIN + EPSILON_OFFSET;

MEMORY {
  PERSITING_BYTES (rx) : ORIGIN = PERSITING_BYTES_ORIGIN, LENGTH = PERSITING_BYTES_LENGTH
  KERNEL (rx) : ORIGIN = KERNEL_ORIGIN, LENGTH = KERNEL_LENGTH
  SRAM_PROCESS_STACK (rw) : ORIGIN = KERNEL_SRAM_ORIGIN - USERLAND_STACK_LENGTH, LENGTH = USERLAND_STACK_LENGTH
  SRAM_STACK (rw) : ORIGIN = KERNEL_SRAM_ORIGIN, LENGTH = KERNEL_STACK_LENGTH
  SRAM_DATA_BSS (rw) : ORIGIN = KERNEL_SRAM_ORIGIN + KERNEL_STACK_LENGTH, LENGTH = KERNEL_SRAM_DATA_BSS_LENGTH
}

SECTIONS {
  INCLUDE flash/prologue.ld

  .text ORIGIN(KERNEL) + SIZEOF(.signed_payload_prefix) + SIZEOF(.kernel_header) + SIZEOF(.isr_vector_table): {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >KERNEL

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >KERNEL

  .rodata : {
    *(.rodata)
    *(.rodata.*)
  } >KERNEL

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
  } >SRAM_DATA_BSS AT> KERNEL

  .fill :
  {
    FILL(0xFF);
    . = ORIGIN(KERNEL) + LENGTH(KERNEL) - 1;
    BYTE(0xFF)
  } > KERNEL

  .persisting_bytes_buffer ORIGIN(PERSITING_BYTES) : {
    _persisting_bytes_buffer_start = .;
    FILL(0xFF);
    /* FILL instruction requires at least one element in the section to fill
     * it. We write the first byte of the persisting bytes to make sure the
     * section is not empty. Also, 0xFF is a forbidden value for the persisting
     * byte (since it is not distinguishable from unused persisting bytes. So
     * we init the first two persisting bytes to be the current significant
     * address. Warning : More bytes should be initialized if
     * PersistingBytes::k_numberOfPersistingBytes is changed. */
    SHORT(0)
    . = ORIGIN(PERSITING_BYTES) + LENGTH(PERSITING_BYTES);
    _persisting_bytes_buffer_end = .;
  } >PERSITING_BYTES

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

  .canary (NOLOAD): {
    INCLUDE flash/canary.ld
  } >SRAM_DATA_BSS

  .process_stack : {
    . = (ORIGIN(SRAM_PROCESS_STACK));
    . = ALIGN(8);
    _process_stack_end = .;
    . += (LENGTH(SRAM_PROCESS_STACK) - 8);
    . = ALIGN(8);
    _process_stack_start = .;
  } >SRAM_PROCESS_STACK

  .main_stack : {
    . = (ORIGIN(SRAM_STACK));
    . = ALIGN(8);
    _main_stack_end = .;
    . += (LENGTH(SRAM_STACK) - 8);
    . = ALIGN(8);
    _main_stack_start = .;
  } >SRAM_STACK

  /DISCARD/ : {
    /* exidx and extab are needed for unwinding, which we don't use */
    *(.ARM.exidx*)
    *(.ARM.extab*)
  }
}

