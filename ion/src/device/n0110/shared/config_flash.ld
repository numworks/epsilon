INCLUDE shared_config_flash.ld

/*
 * |                      INTERNAL FLASH                                                         |
 * | BOOTLOADER | RESCUE BOOTLOADER |  NVIC - PARTIAL KERNEL - NOISE - SIGNATURE INTERNAL KERNEL |
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

INTERNAL_FLASH_KERNEL_ORIGIN = INTERNAL_FLASH_ORIGIN + 2*INTERNAL_FLASH_BOOTLOADER_LENGTH;
INTERNAL_FLASH_KERNEL_LENGTH = INTERNAL_FLASH_LENGTH - 2*INTERNAL_FLASH_BOOTLOADER_LENGTH - SIGNATURE_LENGTH;

/*
 * |                                            EXTERNAL FLASH                                                 |
 * |       4K                |                    7*4K + 32K                     |           127*64K           |
 * | PERSISTING BYTES        |                       KERNEL                      |           USERLAND          |
 * |       4K                |          4K          |        6*4K + 32K          |           127*64K           |
 * | PERSISTING BYTES BUFFER | UNPRIVILEGED KERNEL* | KERNEL | NOISE | SIGNATURE | SIZE - USERLAND - SIGNATURE |
 *
 * *UNPRIVILEGED KERNEL is the only subsection of the kernel which is RW by
 * unprivileged software. We put there only the code responsible for the switch
 * from privileged to unprivileged mode and jumping to userland.
 *
 */

EXTERNAL_FLASH_ORIGIN = 0x90000000;
EXTERNAL_FLASH_LENGTH = 8M;
SMALLEST_EXTERNAL_FLASH_SECTOR_SIZE = 4K;

PERSITING_BYTES_ORIGIN = EXTERNAL_FLASH_ORIGIN;
PERSITING_BYTES_LENGTH = SMALLEST_EXTERNAL_FLASH_SECTOR_SIZE;

EXTERNAL_FLASH_KERNEL_SECTOR_SIZE = 8*4K+32K - PERSITING_BYTES_LENGTH; /* TODO EMILIE: try with less (Cf comment n0110/kernel/drivers/config/board.h) */
EXTERNAL_FLASH_UNPRIVILEGED_KERNEL_ORIGIN = PERSITING_BYTES_ORIGIN + PERSITING_BYTES_LENGTH;
/* The length of the unprivileged kernel is chosen to be a 8th of the
 * privileged-only external flash to be able to use a mask to configurate the
 * MPU. */
EXTERNAL_FLASH_UNPRIVILEGED_KERNEL_LENGTH = 4K;
EXTERNAL_FLASH_PRIVILEGED_KERNEL_ORIGIN = EXTERNAL_FLASH_UNPRIVILEGED_KERNEL_ORIGIN + EXTERNAL_FLASH_UNPRIVILEGED_KERNEL_LENGTH;
/* External flash kernel length is computed to be hold within the 8x4k+1x32k
 * first sectors. */
EXTERNAL_FLASH_PRIVILEGED_KERNEL_LENGTH = EXTERNAL_FLASH_KERNEL_SECTOR_SIZE - EXTERNAL_FLASH_UNPRIVILEGED_KERNEL_LENGTH - SIGNATURE_LENGTH;

FLASH_USERLAND_ORIGIN = EXTERNAL_FLASH_PRIVILEGED_KERNEL_ORIGIN + EXTERNAL_FLASH_PRIVILEGED_KERNEL_LENGTH + SIGNATURE_LENGTH + SIZE_LENGTH;
FLASH_USERLAND_LENGTH = EXTERNAL_FLASH_LENGTH - PERSITING_BYTES_LENGTH - EXTERNAL_FLASH_KERNEL_SECTOR_SIZE - SIZE_LENGTH - SIGNATURE_LENGTH;
