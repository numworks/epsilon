/* Create a firmware that runs from RAM.
 * This is used to:
 *  - Flash faster. Flashing using ST's ROMed DFU bootloader is reliable but
 *  very slow. To make flashing faster, we can leverage ST's bootloader to copy
 *  a small "flasher" in RAM, and run it from there.
 * - Run the bench software from the RAM */

ASSERT((CONFIG_OFFSET + CONFIG_LENGTH <= SRAM_LENGTH), "Error: CONFIG_OFFSET and CONFIG_LENGTH are not compatible");

MEMORY {
  RAM_BUFFER (rw) : ORIGIN = SRAM_ORIGIN + CONFIG_OFFSET, LENGTH = CONFIG_LENGTH
}

/* The stack is quite large: we put it equal to Epsilon's.
 * Indeed, when building the flasher, we're making the USB::Calculator object
 * live on the stack, and it's quite large (about 4K just for this single
 * object). Using a stack too small would result in some memory being
 * overwritten (for instance, vtables that live in the .rodata section). */

STACK_SIZE = 32K;
