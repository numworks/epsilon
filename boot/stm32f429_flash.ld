/* Linker script
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements (for example, we want the code to be
 * written at a specific address (in Flash ROM) and the data at another. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">FLASH" to ask for a given section
 * to be stored in Flash. */
MEMORY {
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 192K
  CCM (rwx)  : ORIGIN = 0x10000000, LENGTH = 64K
}

SECTIONS {
  .isr_vector_table ORIGIN(FLASH) : {
    /* When booting, the STM32F42xx fetches the content of address 0x0, and
     * extracts from it various key infos: the initial value of the PC register
     * (program counter), the initial value of the stack pointer, and various
     * entry points for interrupt service routines. This data is called the
     * ISR vector table.
     *
     * Note that address 0x0 is always an alias. It points to the beginning of
     * Flash, SRAM, or integrated bootloader depending on the boot mode chosen.
     * (This mode is chosen by setting BOOTn pins on the chip.
     *
     * We're generating the ISR vector table in C, because it's very convenient:
     * using function pointers, we can easily point to the service routing for
     * each interrupt.
     * See ST/RM0090/p70 for more infos. */
    *(.isr_vector_table)
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
     *
     * This is required because its initial value matters (so it has to be in
     * persistant memory in the first place), but it is a R/W area of memory
     * so it will have to live in RAM upon execution (in linker lingo, that
     * translate to the data segment having a LMA in Flash and a VMA in RAM).
     *
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
}
