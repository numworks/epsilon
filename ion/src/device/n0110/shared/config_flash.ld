INCLUDE shared_config_flash.ld

/*
 * |                INTERNAL FLASH                |
 * | SECTOR 1 | SECTOR 2 | SECTOR 3 | SECTOR 4    |
 * |           BOOTLOADER           |  TRAMPOLINE |
 */

INTERNAL_FLASH_ORIGIN = 0x00200000;
INTERNAL_FLASH_LENGTH = 64K;
INTERNAL_FLASH_SECTOR_LENGTH = 8K;

BOOTLOADER_ORIGIN = INTERNAL_FLASH_ORIGIN;
BOOTLOADER_LENGTH = INTERNAL_FLASH_LENGTH - INTERNAL_FLASH_SECTOR_LENGTH;
BOOTLOADER_TRAMPOLINE_ORIGIN = INTERNAL_FLASH_ORIGIN + INTERNAL_FLASH_LENGTH - INTERNAL_FLASH_SECTOR_LENGTH;
BOOTLOADER_TRAMPOLINE_LENGTH = INTERNAL_FLASH_SECTOR_LENGTH;

/*
 * |                                             EXTERNAL FLASH                           |
 * |     8*4K+32K     |   4*64K  |   48*64K   |   11*64k    ||       64K        |   4*64K  |   48*64K   |    11*64k     |
 * | PERSISTING BYTES | KERNEL A | USERLAND A | EXTERN APPS || PERSISTING BYTES | KERNEL B | USERLAND B | EXTERNAL APPS |
 *
 * Kernel structure: | NVIC | KERNEL CODE | 0x11..1111 |
 * Userland structure: | SIZE | USERLAND CODE |SIGNATURE | NOISE |
 *
 * The vector table has specific alignment requirements:
 * https://www.st.com/resource/en/programming_manual/dm00237416-stm32f7-series-and-stm32h7-series-cortexm7-processor-programming-manual-stmicroelectronics.pdf
 * We can't precede the kernel with the kernel+userland size because it would
 * create an unconvenient 4-byte offset that would spoil 124 bytes to realign
 * NVIC. Also, it's really convenient to jump from the bootloader to the kernel
 * by using the value kept in the first address of the NVIC which is pratically
 * the first address of the kernel binary.
 *
 */

EXTERNAL_FLASH_ORIGIN = 0x90000000;
EXTERNAL_FLASH_LENGTH = 8M;
STANDARD_EXTERNAL_FLASH_SECTOR_LENGTH = 64K;

PERSITING_BYTES_VIRTUAL_ORIGIN = EXTERNAL_FLASH_ORIGIN;
PERSITING_BYTES_LENGTH = STANDARD_EXTERNAL_FLASH_SECTOR_LENGTH;

KERNEL_VIRTUAL_ORIGIN = PERSITING_BYTES_VIRTUAL_ORIGIN + PERSITING_BYTES_LENGTH;
KERNEL_LENGTH = 4*64K; /* TODO EMILIE: try with less (Cf comment n0110/shared/drivers/config/board.h) */

EXTERNAL_APPS_LENGTH = 11*64K; /* TODO? Change in n0110/shared/drivers/config/board.h)*/
USERLAND_VIRTUAL_ORIGIN = KERNEL_VIRTUAL_ORIGIN + KERNEL_LENGTH + SIZE_LENGTH;
USERLAND_LENGTH = EXTERNAL_FLASH_LENGTH / 2 - PERSITING_BYTES_LENGTH - KERNEL_LENGTH - SIZE_LENGTH - SIGNATURE_LENGTH - EXTERNAL_APPS_LENGTH;
EXTERNAL_APPS_VIRTUAL_ORIGIN = EXTERNAL_FLASH_ORIGIN + EXTERNAL_FLASH_LENGTH / 2 - EXTERNAL_APPS_LENGTH;
