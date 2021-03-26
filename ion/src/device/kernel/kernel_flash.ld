/* Linker script
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements (for example, we want the code to be
 * written at a specific address (in Flash ROM) and the data at another. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">INTERNAL_FLASH" to ask for a given section to
 * be stored in Flash. */

PERSITING_BYTES_ORIGIN = PERSITING_BYTES_VIRTUAL_ORIGIN + EPSILON_OFFSET;
KERNEL_ORIGIN = KERNEL_VIRTUAL_ORIGIN + EPSILON_OFFSET;

MEMORY {
  PERSITING_BYTES (rx) : ORIGIN = PERSITING_BYTES_ORIGIN, LENGTH = PERSITING_BYTES_LENGTH
  KERNEL (rx) : ORIGIN = KERNEL_ORIGIN, LENGTH = KERNEL_LENGTH
  SRAM_DATA_BSS (rw) : ORIGIN = KERNEL_SRAM_ORIGIN, LENGTH = KERNEL_SRAM_DATA_BSS_LENGTH
  SRAM_HEAP_STACK (rw) : ORIGIN = KERNEL_SRAM_ORIGIN + KERNEL_SRAM_DATA_BSS_LENGTH, LENGTH = SRAM_LENGTH - KERNEL_SRAM_DATA_BSS_LENGTH
}

SECTIONS {
  .persisting_bytes_buffer ORIGIN(PERSITING_BYTES) : {
    _persisting_bytes_buffer_start = .;
    KEEP(*(.persisting_bytes_buffer))
    /* Note: We don't increment "." here, we set it. */
    . = ORIGIN(PERSITING_BYTES) + LENGTH(PERSITING_BYTES);
    _persisting_bytes_buffer_end = .;
  } >PERSITING_BYTES

  .isr_vector_table : AT(ORIGIN(KERNEL)) {
    _kernel_start = LOADADDR(.isr_vector_table);
    _isr_vector_table_start_flash = LOADADDR(.isr_vector_table);
    _isr_vector_table_start_ram = .;
    KEEP(*(.isr_vector_table))
    _isr_vector_table_end_ram = .;
  } >SRAM_DATA_BSS

  .text ORIGIN(KERNEL) + SIZEOF (.isr_vector_table) : {
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
    /* Slot information goes right after the isr_vector_table to be easily
     * retrieved from DFU request.*/
    *(.slot_info*)
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

  .heap : {
    _heap_start = .;
    /* Note: We don't increment "." here, we set it. */
    . = (ORIGIN(SRAM_HEAP_STACK) + LENGTH(SRAM_HEAP_STACK) - STACK_LENGTH);
    _heap_end = .;
  } >SRAM_HEAP_STACK

  .stack : {
    . = ALIGN(8);
    _stack_end = .;
    . += (STACK_LENGTH - 8);
    . = ALIGN(8);
    _stack_start = .;
  } >SRAM_HEAP_STACK

  /DISCARD/ : {
    /* exidx and extab are needed for unwinding, which we don't use */
    *(.ARM.exidx*)
    *(.ARM.extab*)
  }
}
