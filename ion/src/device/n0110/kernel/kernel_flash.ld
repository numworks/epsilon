/* Linker script
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements (for example, we want the code to be
 * written at a specific address (in Flash ROM) and the data at another. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">INTERNAL_FLASH" to ask for a given section to
 * be stored in Flash. */

INCLUDE config_flash.ld

MEMORY {
  INTERNAL_FLASH_KERNEL (rx) : ORIGIN = INTERNAL_FLASH_KERNEL_ORIGIN, LENGTH = INTERNAL_FLASH_KERNEL_LENGTH
  EXTERNAL_FLASH_PERSITING_BYTES (rx) : ORIGIN = PERSITING_BYTES_ORIGIN, LENGTH = PERSITING_BYTES_LENGTH
  EXTERNAL_FLASH_KERNEL (rx) : ORIGIN = EXTERNAL_FLASH_KERNEL_ORIGIN, LENGTH = EXTERNAL_FLASH_KERNEL_LENGTH
  SRAM_DATA_BSS (rw) : ORIGIN = KERNEL_SRAM_ORIGIN, LENGTH = KERNEL_SRAM_DATA_BSS_LENGTH
  SRAM_HEAP_STACK (rw) : ORIGIN = KERNEL_SRAM_ORIGIN + KERNEL_SRAM_DATA_BSS_LENGTH, LENGTH = SRAM_LENGTH - KERNEL_SRAM_DATA_BSS_LENGTH
}


SECTIONS {
  .isr_vector_table ORIGIN(INTERNAL_FLASH_KERNEL) : {
    _isr_start = .;
    KEEP(*(.isr_vector_table))
  } >INTERNAL_FLASH_KERNEL

  .header : {
    KEEP(*(.header))
  } >INTERNAL_FLASH_KERNEL
  /* Internal flash memory */

  /* We link 'jump_to_external_flash' in a separate section from text.internal.
   * Indeed, it is the only internal flash symbol that refers the external
   * flash. This way we can assert that there are no cross references from the
   * internal flash to the external flash. */
  .text.internal_to_external : {
    *(.text._ZL22jump_to_external_flashv*)
  } >INTERNAL_FLASH_KERNEL

  /* Use boot routine and required dependencies */
  /* We're relying on symbols being in their own sub-section. On GCC, this is
   * done with -fdata-sections -ffunction-sections */
  .text.internal : {
    . = ALIGN(4);
    *(.text.start)
    *(.text.abort)
    *(.text.isr_systick)
    *(.text.__assert)
    *(.text.memcpy)
    *(.text.memset)
    /*
    *(.text.strlen)
    *(.text.strncmp)
    *(.text.strlcpy)
    *(.text.strcmp)
    *(.text.memmove)
    */
    /* 'start' dependencies */
    *(.text._ZN3Ion6Device5Board4initEv)
    *(.text._ZN3Ion6Device5Board7initFPUEv)
    *(.text._ZN3Ion6Device5Board7initMPUEv)
    *(.text._ZN3Ion6Device5Board21initPeripheralsClocksEv)
    *(.text._ZN3Ion6Device5Board17initInterruptionsEv)
    *(.text._ZN3Ion6Device4Regs*)
    *(.text._ZNK3Ion6Device4Regs*)
    *(.text._ZNV3Ion6Device4Regs*)
    *(.text.*3Ion6Device4Regs4GPIO5PUPDR*)
    *(.text._ZNK3Ion6Device4Regs9AFGPIOPin*)
    *(.text._ZN3Ion6Device13ExternalFlash*)
    *(.text._ZZN3Ion6Device13ExternalFlash*)
    *(.text._ZN3Ion6Device5Cache*)
    *(.text._ZN3Ion6Device6Timing6usleepEj)
    *(.text._ZN3Ion6Device4Init12configureRAMEv)
    *(.text._ZN3Ion6Device5Board14setDefaultGPIOEv)

    /* keyboard_handler does not need to be in internal flash, the interruption
     * is activated after initializing the external flash. */

    /* 'abort' dependencies */
    *(.text._ZN3Ion6Device5Reset4coreEv)

    /* 'standby' dependencies '*/
    *(.text._ZN3Ion6Device5Power20internalFlashStandbyEv)
    *(.text._ZN3Ion6Device5Power17enterLowPowerModeEv)
    *(.text._ZN3Ion6Device5Board14shutdownClocksEb)
    *(.text._ZN3Ion6Device5Reset16coreWhilePluggedEv)
    *(.text._ZN3Ion6Device5Board25shutdownPeripheralsClocksEb)
    *(.text._ZN3Ion6Device5Power41shutdownExternalFlashAndEnterLowPowerModeEb)
    *(.text._ZN3Ion6Device5Board21shutdownInterruptionsEv)

    /* 'suspend' dependencies '*/
    *(.text._ZN3Ion6Device5Power20internalFlashSuspendEb)

    /* 'Reset::jump' dependencies '*/
    *(.text._ZN3Ion6Device5Reset17internalFlashJumpEj)

    /* Optimization */
    */libgcc.a:(.text)
  } >INTERNAL_FLASH_KERNEL


  .rodata.internal : {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata._ZN3Ion6Device13ExternalFlash*)
    /* 'start' dependencies */
    *(.rodata._ZN3Ion6Device4RegsL5GPIOAE)
    *(.rodata._ZN3Ion6Device4RegsL5GPIOBE)
    *(.rodata._ZN3Ion6Device5Board4initEv.str1.4)
    *(.rodata._ZN3Ion6Device8Keyboard18handleInterruptionEv.str1.4)
    *(.rodata._ZN3Ion6Device8Keyboard6ConfigL10ColumnPinsE)
    *(.rodata._ZN3Ion6Device8Keyboard6ConfigL7RowPinsE)
    *(.rodata._ZN3Ion6Device5Cache35privateCleanInvalidateDisableDCacheEbbb.str1.4)
    *(.rodata._ZN3Ion6Device4Regs*)
    *(.rodata._ZN3Ion6Device8Keyboard6Config*)
    *(.rodata._ZN3Ion6Device5BoardL17interruptionIndexE)
  } >INTERNAL_FLASH_KERNEL

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >INTERNAL_FLASH_KERNEL

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
  } >SRAM_DATA_BSS AT> INTERNAL_FLASH_KERNEL

  .fill.internal :
  {
    FILL(0xFF);
    . = ORIGIN(INTERNAL_FLASH_KERNEL) + LENGTH(INTERNAL_FLASH_KERNEL) - 1;
    BYTE(0xFF)
  } > INTERNAL_FLASH_KERNEL

  /* External flash memory */

  .persisting_bytes_buffer ORIGIN(EXTERNAL_FLASH_PERSITING_BYTES) : {
    _persisting_bytes_buffer_start = .;
    KEEP(*(.persisting_bytes_buffer))
    /* Note: We don't increment "." here, we set it. */
    . = ORIGIN(EXTERNAL_FLASH_PERSITING_BYTES) + LENGTH(EXTERNAL_FLASH_PERSITING_BYTES);
    _persisting_bytes_buffer_end = .;
  } >EXTERNAL_FLASH_PERSITING_BYTES

  .text.external ORIGIN(EXTERNAL_FLASH_KERNEL) : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >EXTERNAL_FLASH_KERNEL

  .rodata.external : {
    *(.rodata)
    *(.rodata.*)
  } >EXTERNAL_FLASH_KERNEL

  .fill.external :
  {
    FILL(0xFF);
    . = ORIGIN(EXTERNAL_FLASH_KERNEL) + LENGTH(EXTERNAL_FLASH_KERNEL) - 1;
    BYTE(0xFF)
  } > EXTERNAL_FLASH_KERNEL

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
    . = (ORIGIN(SRAM_HEAP_STACK) + LENGTH(SRAM_HEAP_STACK) - STACK_SIZE);
    _heap_end = .;
  } >SRAM_HEAP_STACK

  .stack : {
    . = ALIGN(8);
    _stack_end = .;
    . += (STACK_SIZE - 8);
    . = ALIGN(8);
    _stack_start = .;
  } >SRAM_HEAP_STACK

  /DISCARD/ : {
    /* exidx and extab are needed for unwinding, which we don't use */
    *(.ARM.exidx*)
    *(.ARM.extab*)
  }
}

NOCROSSREFS_TO(.text.external .text.internal);
NOCROSSREFS_TO(.rodata.external .text.internal);
NOCROSSREFS_TO(.text.external .rodata.internal);
NOCROSSREFS_TO(.rodata.external .rodata.internal);
NOCROSSREFS_TO(.text.external .header);
NOCROSSREFS_TO(.rodata.external .header);

NOCROSSREFS_TO(.persisting_bytes_buffer .text.internal);
NOCROSSREFS_TO(.persisting_bytes_buffer .rodata.internal);
NOCROSSREFS_TO(.persisting_bytes_buffer .isr_vector_table);
NOCROSSREFS_TO(.persisting_bytes_buffer .header);
