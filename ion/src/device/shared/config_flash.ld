SIZE_LENGTH = 4;
SIGNATURE_LENGTH = 32;

/*
 * |                      INTERNAL FLASH                                                 |
 * | BOOTLOADER | RESCUE BOOTLOADER |  NVIC - PARTIAL KERNEL - SIGNATURE INTERNAL KERNEL |
 *
 * The vector table has specific alignment requirements:
 * https://www.st.com/resource/en/programming_manual/dm00237416-stm32f7-series-and-stm32h7-series-cortexm7-processor-programming-manual-stmicroelectronics.pdf
 * We can't precede the partial kernel with its size because it would create an
 * unconvenient 4-byte offset that would spoil 124 bytes to realign NVIC. Also,
 * it's really convenient to jump from the bootloader to the kernel by using
 * the value kept in the first address of the NVIC which is pratically the
 * first address of the kernel binary.
 *
 */

INTERNAL_FLASH_ORIGIN = 0x00200000;
INTERNAL_FLASH_LENGTH = 64K;

INTERNAL_FLASH_BOOTLOADER_LENGTH = 16K;

INTERNAL_FLASH_KERNEL_ORIGIN = INTERNAL_FLASH_ORIGIN + 2*INTERNAL_FLASH_BOOTLOADER_LENGTH;
INTERNAL_FLASH_KERNEL_LENGTH = INTERNAL_FLASH_LENGTH - 2*INTERNAL_FLASH_BOOTLOADER_LENGTH - SIGNATURE_LENGTH;

/*
 * |                               EXTERNAL FLASH                                       |
 * | KERNEL                                               | USERLAND                    |
 * | EXAM MODE BUFFER - SIZE - PARTIAL KERNEL - SIGNATURE | SIZE - USERLAND - SIGNATURE |
 *
 */

EXTERNAL_FLASH_ORIGIN = 0x90000000;
EXTERNAL_FLASH_LENGTH = 8M;
FIRST_EXTERNAL_FLASH_SECTOR_SIZE = 4K;

EXAM_MODE_ORIGIN = EXTERNAL_FLASH_ORIGIN;
EXAM_MODE_LENGTH = FIRST_EXTERNAL_FLASH_SECTOR_SIZE;

EXTERNAL_FLASH_KERNEL_ORIGIN = EXAM_MODE_ORIGIN + EXAM_MODE_LENGTH + SIZE_LENGTH;
EXTERNAL_FLASH_KERNEL_LENGTH = 8K; /* TODO EMILIE: This might be to much */

EXTERNAL_FLASH_USERLAND_ORIGIN = EXTERNAL_FLASH_KERNEL_ORIGIN + EXTERNAL_FLASH_KERNEL_LENGTH + SIGNATURE_LENGTH + SIZE_LENGTH;
EXTERNAL_FLASH_USERLAND_LENGTH = EXTERNAL_FLASH_LENGTH - EXAM_MODE_LENGTH - EXTERNAL_FLASH_KERNEL_LENGTH - 2*SIZE_LENGTH - 2*SIGNATURE_LENGTH;

/*
 * |                                      SRAM                                       |
 * | KERNEL DATA AND BSS | EXTERNAL FLASH DATA AND BSS | HEAP | SHARED STACK |
 *
 */

SRAM_ORIGIN = 0x20000000;
SRAM_LENGTH = 256K;
STACK_SIZE = 32K;

KERNEL_SRAM_ORIGIN = SRAM_ORIGIN;
KERNEL_SRAM_DATA_BSS_LENGTH = 16K; /* TODO EMILIE */
USERLAND_SRAM_ORIGIN = KERNEL_SRAM_ORIGIN + KERNEL_SRAM_DATA_BSS_LENGTH;
USERLAND_SRAM_LENGTH = SRAM_LENGTH - KERNEL_SRAM_DATA_BSS_LENGTH;

/*
 ITCM (rwx) : ORIGIN = 0x00000000, LENGTH = 16K
 DTCM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
 SRAM1 (rwx) : ORIGIN = 0x20010000, LENGTH = 176K
 SRAM2 (rwx) : ORIGIN = 0x2003C000, LENGTH = 16K
*/

