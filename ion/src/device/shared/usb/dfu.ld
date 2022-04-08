/* DFU transfers can serve two purposes:
 *  - Transferring RAM data between the machine and the host, e.g. Python scripts
 *  - Upgrading the flash memory to perform a software update
 *
 * The second case raises a huge issue: code cannot be executed from memory that
 * is being modified. We're solving this issue by copying the DFU code in RAM.
 *
 * This linker script will generate some code that expects to be executed from a
 * fixed address in RAM. The corresponding instructions will be embedded in the
 * main Epsilon ELF file, and copied to that address before execution.
 *
 * This address needs to live in RAM, and needs to be temporarily overwriteable
 * when the program is being run. Epsilon has a large stack to allow deeply
 * recursive code to run. But when doing DFU transfers it is safe to assume we
 * will need very little stack space. We're therefore using the topmost 8K of
 * the stack reserved by Epsilon.
 *
 * Last but not least, we'll want to jump to a known entry point when running
 * the DFU code (namely, Ion::USB::Device::Calculator::Poll). We're simply
 * making sure this is the first symbol output. */

EPSILON_STACK_END = 0x20000000 + 256K - 32K;

MEMORY {
  RAM_BUFFER (rw) : ORIGIN = EPSILON_STACK_END, LENGTH = 9K
}

SECTIONS {
  .text : {
    . = ALIGN(4);
    KEEP(*(.dfu_entry_point))
    *(.text)
    *(.text.*)
  } >RAM_BUFFER

  .rodata : {
    *(.rodata)
    *(.rodata.*)
  } >RAM_BUFFER

  .data : {
    /* We need to keep these symbols. */
    *(.data._ZN3Ion6Device13ExternalFlashL14sOperatingModeE)
    *(.data._ZN3Ion6Device5BoardL18sStandardFrequencyE)
  } >RAM_BUFFER

  /DISCARD/ : {
    /* For now, we do not need .bss and .data sections. This allows us to simply
     * skip any rt0-style initialization and jump straight into the PollAndReset
     * routine. */
    *(.bss)
    *(.bss.*)
    *(.data)
    *(.data.*)
  }
}

