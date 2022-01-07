. = ALIGN(64K);
_external_apps_flash_start = .;
. = (ORIGIN(FLASH_USERLAND) + LENGTH(FLASH_USERLAND));
_external_apps_flash_end = .;
