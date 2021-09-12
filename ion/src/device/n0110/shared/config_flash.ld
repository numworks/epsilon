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

SLOT_SIZE = 1M+500K;

PERSITING_BYTES_VIRTUAL_ORIGIN = EXTERNAL_FLASH_ORIGIN + SLOT_SIZE - STANDARD_EXTERNAL_FLASH_SECTOR_LENGTH;
PERSITING_BYTES_LENGTH = STANDARD_EXTERNAL_FLASH_SECTOR_LENGTH;

KERNEL_VIRTUAL_ORIGIN = EXTERNAL_FLASH_ORIGIN;
KERNEL_LENGTH = 64K;

USERLAND_VIRTUAL_ORIGIN = KERNEL_VIRTUAL_ORIGIN + KERNEL_LENGTH;
USERLAND_LENGTH = SLOT_SIZE - PERSITING_BYTES_LENGTH - KERNEL_LENGTH;

STORAGE_ORIGIN = EXTERNAL_FLASH_ORIGIN + 2 * SLOT_SIZE;
STORAGE_SIZE = STORAGE_ORIGIN - EXTERNAL_FLASH_ORIGIN;


SIGNED_PAYLOAD_LENGTH = 8;
CANARY_LENGTH = 4;

/* Caution: ST's bootloader uses some RAM, so we want to stay off of that memory
 * region. Per AN2606, sections 31.1 and 36.1, it's using 16Kbytes form address
 * 0x20000000. We'll try to play safe and avoid the first 32KB of RAM. */
ST_BOOTLOADER_STACK = 32K;

/* The flasher is linked in RAM from address 0x20030000 to the end. */
FLASHER_LENGTH = 64K;
FLASHER_OFFSET = SRAM_LENGTH - FLASHER_LENGTH;
ASSERT(ST_BOOTLOADER_STACK < FLASHER_OFFSET, "The Flasher overlaps ST bootloader stack")

/* The bench is linked in RAM from address 0x20000000 to 0x2002000.
 * Since it is written by either the flasher of the bootloader, there is no
 * need to keep space for ST bootloader. */
BENCH_LENGTH = 128K;
BENCH_OFFSET = 0;
