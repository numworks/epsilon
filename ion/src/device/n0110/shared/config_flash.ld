INCLUDE shared_config_flash.ld

/*
 * | INTERNAL FLASH |
 * |   BOOTLOADER   |
 */

INTERNAL_FLASH_ORIGIN = 0x00200000;
INTERNAL_FLASH_LENGTH = 64K;

BOOTLOADER_LENGTH = 64K;

/*
 * |                                             EXTERNAL FLASH                           |
 * |     8*4K+32K     |   4*64K  |   59*64K   ||       64K        |   4*64K  |   59*64K   |
 * | PERSISTING BYTES | KERNEL A | USERLAND A || PERSISTING BYTES | KERNEL B | USERLAND B |
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
STANDARD_EXTERNAL_FLASH_SECTOR_SIZE = 64K;

PERSITING_BYTES_OFFSET = 0x0;
PERSITING_BYTES_LENGTH = STANDARD_EXTERNAL_FLASH_SECTOR_SIZE;

KERNEL_OFFSET = PERSITING_BYTES_OFFSET + PERSITING_BYTES_LENGTH;
KERNEL_SIZE = 4*64K; /* TODO EMILIE: try with less (Cf comment n0110/kernel/drivers/config/board.h) */

USERLAND_OFFSET = KERNEL_OFFSET + KERNEL_SIZE + SIZE_LENGTH;
USERLAND_LENGTH = EXTERNAL_FLASH_LENGTH - PERSITING_BYTES_LENGTH - KERNEL_SIZE - SIZE_LENGTH - SIGNATURE_LENGTH;
