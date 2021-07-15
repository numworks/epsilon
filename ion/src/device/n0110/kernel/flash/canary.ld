. = (ORIGIN(SRAM_DATA_BSS) + LENGTH(SRAM_DATA_BSS) - CANARY_LENGTH);
. = ALIGN(4);
_canary_section_start_ram = .;
KEEP(*(.canary))
_canary_section_end_ram = .;
ASSERT ((_canary_section_start_ram + CANARY_LENGTH == _canary_section_end_ram), "Kernel canary section should only contain the canary");
