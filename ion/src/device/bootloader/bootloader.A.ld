/* Linker script
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements (for example, we want the code to be
 * written at a specific address (in Flash ROM) and the data at another. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">FLASH" to ask for a given section to
 * be stored in Flash. */

MEMORY {
  SRAM (rw)  : ORIGIN = 0x20000000, LENGTH = 256K
  FLASH (rx) : ORIGIN = 0x90000000, LENGTH = 4M
  /*
  ITCM (rwx) : ORIGIN = 0x00000000, LENGTH = 16K
  DTCM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
  SRAM1 (rwx) : ORIGIN = 0x20010000, LENGTH = 176K
  SRAM2 (rwx) : ORIGIN = 0x2003C000, LENGTH = 16K
   */
}

STACK_SIZE = 32K;
FIRST_FLASH_SECTOR_SIZE = 4K;
SIGNED_PAYLOAD_LENGTH = 8;
USERLAND_OFFSET = 64K;

INCLUDE ion/src/device/bootloader/bootloader_common.ld;
