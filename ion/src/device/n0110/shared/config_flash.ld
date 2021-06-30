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
 * |                                             EXTERNAL FLASH                                                                                 |
 *  |   4*64K  |           59*64K         |        64K                                ||   4*64K  |            59*64K        |         64k      |
 * ||                  SIGNED PAYLOAD                | EXTERN APPS | PERSISTING BYTES ||    SIGNED PAYLOAD     | EXTERN APPS | PERSISTING BYTES ||
 * || SP* HEADER | KERNEL A | USERLAND A | SIGNATURE | EXTERN APPS | PERSISTING BYTES ||               ... Identical slot B ...                 ||
 *
 * SIGNED PAYLOAD structure:| SP HEADER |                           DATA                                       | SIGNATURE |
 * KERNEL structure:                     | HEADER | INIT VECTOR | CODE | 0x11..1 |
 * USERLAND structure:                                                           | HEADER | INIT VECTOR | CODE |
 *
 * Kernel and Userland are prefixed by:
 * - a header listing various information that have to be retrieved from DFU
 * - a initialisation vector starting with the entry point and the stack initialization
 *
 * The kernel initialisation vector table has specific requirements regarding:
 * - its memory space: it must be in the range 0x00000080 to 0x3FFFFF80
 * - its alignement: you must align the offset to the number of exception entries in the vector table. The minimum alignment is 128 words.
 * [https://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf]
 * Therefore, its is also relocated in sRAM at booting.
 *
 */

EXTERNAL_FLASH_ORIGIN = 0x90000000;
EXTERNAL_FLASH_LENGTH = 8M;
STANDARD_EXTERNAL_FLASH_SECTOR_LENGTH = 64K;

PERSITING_BYTES_VIRTUAL_ORIGIN = EXTERNAL_FLASH_ORIGIN + EXTERNAL_FLASH_LENGTH / 2 - STANDARD_EXTERNAL_FLASH_SECTOR_LENGTH;
PERSITING_BYTES_LENGTH = STANDARD_EXTERNAL_FLASH_SECTOR_LENGTH;

SIGNED_PAYLOAD_HEADER = 8;

KERNEL_VIRTUAL_ORIGIN = EXTERNAL_FLASH_ORIGIN + SIGNED_PAYLOAD_HEADER;
KERNEL_LENGTH = 64K - SIGNED_PAYLOAD_HEADER; /* TODO EMILIE: try with less (Cf comment n0110/shared/drivers/config/board.h) */

USERLAND_VIRTUAL_ORIGIN = KERNEL_VIRTUAL_ORIGIN + KERNEL_LENGTH;
USERLAND_AND_EXTERNAL_APPS_LENGTH = EXTERNAL_FLASH_LENGTH / 2 - SIGNED_PAYLOAD_HEADER - KERNEL_LENGTH - PERSITING_BYTES_LENGTH;
