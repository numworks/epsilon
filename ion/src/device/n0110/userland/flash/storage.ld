. = ALIGN(64K);
_storage_flash_start = .;
. = (ORIGIN(FLASH_USERLAND) + LENGTH(FLASH_USERLAND));
_storage_flash_end = .;
