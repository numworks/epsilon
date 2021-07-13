/* Create a firmware that runs from RAM.
 * This is used to:
 *  - Flash faster. Flashing using ST's ROMed DFU bootloader is reliable but
 *  very slow. To make flashing faster, we can leverage ST's bootloader to copy
 *  a small "flasher" in RAM, and run it from there.
 * TODO : Eventually, this may also be used to run the bench software from the RAM */

INCLUDE config/ram.ld

/* Caution: ST's bootloader uses some RAM, so we want to stay off of that memory
 * region. Per AN2606, sections 31.1 and 36.1, it's using 16Kbytes form address
 * 0x20000000. We'll try to play safe and avoid the first 32KB of RAM.*/
ASSERT ((CONFIG_OFFSET >= 0), "Error: No room left for ST's bootloader");
ASSERT ((CONFIG_OFFSET + CONFIG_LENGTH <=  256K - 32K), "Error: CONFIG_OFFSET and CONFIG_LENGTH are not compatible");
MEMORY {
  RAM_BUFFER (rw) : ORIGIN = 0x20000000 + 32K + CONFIG_OFFSET, LENGTH = CONFIG_LENGTH
}

/* The stack is quite large: we put it equal to Epsilon's.
 * Indeed, when building the flasher, we're making the USB::Calculator object
 * live on the stack, and it's quite large (about 4K just for this single
 * object). Using a stack too small would result in some memory being
 * overwritten (for instance, vtables that live in the .rodata section). */

STACK_SIZE = 32K;

SECTIONS {
  .isr_vector_table ORIGIN(RAM_BUFFER) : {
    _isr_start = .;
    KEEP(*(.isr_vector_table))
  } >RAM_BUFFER

  .text : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >RAM_BUFFER

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >RAM_BUFFER

  .rodata : {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata.*)
  } >RAM_BUFFER

  .data : {
    *(.data)
    *(.data.*)
  } >RAM_BUFFER
  ASSERT(SIZEOF(.data) == 0, "Flasher .data section is not empty")

  .bss : {
    *(.bss)
    *(.bss.*)
    *(COMMON)
  } >RAM_BUFFER
  ASSERT(SIZEOF(.bss) == 0, "Flasher .bss section is not empty")

  .stack : {
    . = ALIGN(8);
    _stack_end = .;
    . += (STACK_SIZE - 8);
    . = ALIGN(8);
    _stack_start = .;
  } >RAM_BUFFER
}
