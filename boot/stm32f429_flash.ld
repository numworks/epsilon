MEMORY {
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 192K
  CCM (rwx)  : ORIGIN = 0x10000000, LENGTH = 64K
}

/* We want the code at symbol "reset" to be laid out at the beginning of the
 * text section */
ENTRY(_start)

SECTIONS {
  .isr_vector_table ORIGIN(FLASH) : {
    /* We're explicitly asking for the ISR vector table to be at the beginning
     * of the Flash memory. This is what the STM32F42xx expects when booting.
     * See ST/RM0090/p70 */
    *(.isr_vector_table)
    /*LONG(0x20010000);*/ /* Stack pointer */
    /*LONG(0x00000201);*/ /* Reset vector */
    /* CAUTION: The least significant bit of the reset
     * vector should be set to indicate Thumb code */
  } >FLASH


  .text : {
    . = ALIGN(4);
    *(.text)
  } >FLASH

  .rodata : {
    . = ALIGN(4);
    *(.rodata)
  } >FLASH

  .data : {
    /* The data section is written to Flash but linked as if it were in RAM.
     * This is required because its initial value matters (so it has to be in
     * persistant memory in the first place), but it is a R/W area of memory
     * so it will have to live in RAM upon execution (in linker lingo, that
     * translate to the data segment having a LMA in Flash and a VMA in RAM).
     * This means we'll have to copy it from Flash to RAM on initialization.
     * To do this, we'll need to know the source location of the data segment
     * (in Flash), the target location (in RAM), and the size of the segment.
     * That's why we're defining three symbols that we'll use in the initial-
     * -ization routine. */
    . = ALIGN(4);
    _data_segment_start_flash = LOADADDR(.data);
    _data_segment_start_ram = .;
    *(.data)
    _data_segment_end_ram = .;
  } >RAM AT> FLASH
  /*. = 0x8000000;
  .data : { *(.data) }
  .bss : { *(.bss) }*/
}
