/* Linker script */

INCLUDE config_flash.ld

MEMORY {
  FLASH (rx) : ORIGIN = 0, LENGTH = NUMBER_OF_EXTERNAL_FLASH_SECTORS*SIZE_OF_EXTERNAL_FLASH_SECTOR
  /* External Apps have no allocated space for any bss or data. */
  SRAM_DATA_BSS (rw) : ORIGIN = 0x20000000, LENGTH = 0
}

SECTIONS {
  .app_info ORIGIN(FLASH) : {
    KEEP(*(.app_info))
  } >FLASH

  .text : {
    . = ALIGN(4);
    _main_symbol = .;
    KEEP(*(*._Z8app_mainv));
    *(.text)
    *(.text.*)
  } >FLASH

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >FLASH

  .rodata : {
    _title_symbol = .;
    KEEP(*(*._ZL5title))
    _icon_symbol = .;
    KEEP(*(*.compressedPixelData))
    *(.rodata)
    *(.rodata.*)
  } >FLASH

  .data : {
    . = ALIGN(4);
    _data_section_start_ram = .;
    *(.data)
    *(.data.*)
    _data_section_end_ram = .;
    _app_flash_end = LOADADDR(.data) + SIZEOF(.data);
    ASSERT ((_data_section_start_ram == _data_section_end_ram), "External app data section must be empty");
  } >SRAM_DATA_BSS AT> FLASH

  .bss : {
    . = ALIGN(4);
    _bss_section_start_ram = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    _bss_section_end_ram = .;
    ASSERT ((_bss_section_start_ram == _bss_section_end_ram), "External app bss section must be empty");
  } >SRAM_DATA_BSS
}

