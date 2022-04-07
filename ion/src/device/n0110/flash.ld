/* Linker script
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements (for example, we want the code to be
 * written at a specific address (in Flash ROM) and the data at another. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">INTERNAL_FLASH" to ask for a given section to
 * be stored in Flash. */

MEMORY {
  INTERNAL_FLASH (rx) : ORIGIN = 0x00200000, LENGTH = 64K
  SRAM (rw)  : ORIGIN = 0x20000000, LENGTH = 256K
  EXTERNAL_FLASH (rx) : ORIGIN = 0x90000000, LENGTH = 8M
  /*
  ITCM (rwx) : ORIGIN = 0x00000000, LENGTH = 16K
  DTCM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
  SRAM1 (rwx) : ORIGIN = 0x20010000, LENGTH = 176K
  SRAM2 (rwx) : ORIGIN = 0x2003C000, LENGTH = 16K
   */
}

STACK_SIZE = 32K;
FIRST_EXTERNAL_FLASH_SECTOR_SIZE = 4K;

SECTIONS {
  .isr_vector_table ORIGIN(INTERNAL_FLASH) : {
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
    KEEP(*(.isr_vector_table))
  } >INTERNAL_FLASH

  .header : {
    KEEP(*(.header))
  } >INTERNAL_FLASH

  /* Internal flash memory */

  /* We link 'jump_to_external_flash' in a separate section from text.internal.
   * Indeed, it is the only internal flash symbol that refers the external
   * flash. This way we can assert that there are no cross references from the
   * internal flash to the external flash. */
  .text.internal_to_external : {
    *(.text._ZL22jump_to_external_flashv*)
  } >INTERNAL_FLASH

  /* Use boot routine and required dependencies */
  /* We're relying on symbols being in their own sub-section. On GCC, this is
   * done with -fdata-sections -ffunction-sections */
  .text.internal : {
    . = ALIGN(4);
    *(.text.start)
    *(.text.abort)
    *(.text.hard_fault_handler)
    *(.text.mem_fault_handler)
    *(.text.usage_fault_handler)
    *(.text.bus_fault_handler)
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
    *(.text._ZN3Ion6Device5Board10initClocksEv)
    *(.text._ZNV3Ion6Device4Regs*)
    *(.text._ZN3Ion6Device4Regs*)
    *(.text.*3Ion6Device4Regs4GPIO5PUPDR*)
    *(.text._ZNK3Ion6Device4Regs9AFGPIOPin*)
    *(.text._ZN3Ion6Device13ExternalFlash*)
    *(.text._ZN3Ion6Device5Cache*)
    *(.text._ZN3Ion6Timing6usleepEj)

    /* 'abort' dependencies */
    *(.text._ZN3Ion6Device5Reset4coreEv)

    /* 'standby' dependencies '*/
    *(.text._ZN3Ion6Device5Power20internalFlashStandbyEv)
    *(.text._ZN3Ion6Device5Power17enterLowPowerModeEv)
    *(.text._ZN3Ion6Device5Board14shutdownClocksEb)
    *(.text._ZN3Ion6Device5Reset16coreWhilePluggedEv)

    /* 'suspend' dependencies '*/
    *(.text._ZN3Ion6Device5Power20internalFlashSuspendEb)

    /* 'Reset::jump' dependencies '*/
    *(.text._ZN3Ion6Device5Reset17internalFlashJumpEj)

    /* Optimization */
    */libgcc.a:(.text)
  } >INTERNAL_FLASH


  .rodata.internal : {
    . = ALIGN(4);
    *(.rodata._ZN3Ion6Device13ExternalFlash*)
    /* 'start' dependencies */
    *(.rodata._ZN3Ion6Device4RegsL5GPIOAE)
    *(.rodata._ZN3Ion6Device4RegsL5GPIOBE)
    *(.rodata._ZN3Ion6Device5Board4initEv.str1.4)
  } >INTERNAL_FLASH

  .exam_mode_buffer ORIGIN(EXTERNAL_FLASH) : {
    _exam_mode_buffer_start = .;
    KEEP(*(.exam_mode_buffer))
    /* Note: We don't increment "." here, we set it. */
    . = ORIGIN(EXTERNAL_FLASH) + FIRST_EXTERNAL_FLASH_SECTOR_SIZE;
    _exam_mode_buffer_end = .;
  } >EXTERNAL_FLASH

  /* External flash memory */
  .text.external : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >EXTERNAL_FLASH

  .rodata.external : {
    *(.rodata)
    *(.rodata.*)
  } >EXTERNAL_FLASH

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >INTERNAL_FLASH

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
  } >SRAM AT> INTERNAL_FLASH

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

NOCROSSREFS_TO(.text.external .text.internal);
NOCROSSREFS_TO(.rodata.external .text.internal);
NOCROSSREFS_TO(.text.external .rodata.internal);
NOCROSSREFS_TO(.rodata.external .rodata.internal);
NOCROSSREFS_TO(.text.external .isr_vector_table);
NOCROSSREFS_TO(.rodata.external .isr_vector_table);
NOCROSSREFS_TO(.text.external .header);
NOCROSSREFS_TO(.rodata.external .header);

NOCROSSREFS_TO(.exam_mode_buffer .text.internal);
NOCROSSREFS_TO(.exam_mode_buffer .rodata.internal);
NOCROSSREFS_TO(.exam_mode_buffer .isr_vector_table);
NOCROSSREFS_TO(.exam_mode_buffer .header);
