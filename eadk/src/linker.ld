MEMORY {
  FLASH (rx) : ORIGIN = {{ flashStart }}, LENGTH = {{ flashLength }}
  RAM (rwx) : ORIGIN = {{ ramStart }}, LENGTH = {{ ramLength }}
}

ENTRY(_start);

SECTIONS {
  .eadk_app_info ORIGIN(FLASH) : {
    LONG(0xDEC0BEBA);
    KEEP(*(.rodata.eadk_api_level))
    LONG(ADDR(.rodata.eadk_app_name) - ORIGIN(FLASH));
    LONG(SIZEOF(.rodata.eadk_app_icon));
    LONG(ADDR(.rodata.eadk_app_icon) - ORIGIN(FLASH));
    LONG(_start - ORIGIN(FLASH));
    LONG(_eadk_app_end - ORIGIN(FLASH));
    LONG(0xDEC0BEBA);
  } >FLASH

  .rodata.eadk_app_name : {
    KEEP(*(.rodata.eadk_app_name))
  } >FLASH

  .rodata.eadk_app_icon : {
    KEEP(*(.rodata.eadk_app_icon))
  } >FLASH

  .text : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >FLASH

  .rodata : {
    *(.rodata)
    *(.rodata.*)
    _eadk_app_end = .;
  } >FLASH

  .data : {
    . = ALIGN(4);
    _data_section_start_flash = LOADADDR(.data);
    _data_section_start_ram = .;
    *(.data)
    *(.data.*)
    _data_section_end_ram = .;
  } >RAM AT >FLASH

  .bss : {
    . = ALIGN(4);
    _bss_section_start_ram = .;
    *(.bss)
    *(.bss.*)
    _bss_section_end_ram = .;
  } >RAM

  .heap : {
    _heap_start = .;
    . = (ORIGIN(RAM) + LENGTH(RAM));
    _heap_end = .;
  } >RAM
}

ASSERT(SIZEOF(.eadk_app_info) == 32, "Missing api level. Please define a .rodata.eadk_api_level section.");
ASSERT(SIZEOF(.rodata.eadk_app_name) > 0, "Missing app name. Please define a .rodata.eadk_app_name section.");
ASSERT(SIZEOF(.rodata.eadk_app_icon) > 0, "Missing app icon. Please define a .rodata.eadk_app_icon section.");
