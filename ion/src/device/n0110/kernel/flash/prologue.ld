.signed_payload_prefix ORIGIN(KERNEL) : {
  FILL(0xFF);
  BYTE(0xFF)
  . = ORIGIN(KERNEL) + SIGNED_PAYLOAD_LENGTH;
} >KERNEL

.kernel_header : {
  KEEP(*(.kernel_header))
} >KERNEL

.isr_vector_table : AT(ORIGIN(KERNEL) + SIZEOF(.signed_payload_prefix) + SIZEOF(.kernel_header)) {
  _isr_vector_table_start_flash = LOADADDR(.isr_vector_table);
  _isr_vector_table_start_ram = .;
  KEEP(*(.isr_vector_table))
  _isr_vector_table_end_ram = .;
} >SRAM_DATA_BSS
