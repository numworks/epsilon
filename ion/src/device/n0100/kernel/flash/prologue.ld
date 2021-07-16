/* Empty section to factor linker script with N0110 */
.signed_payload_prefix ORIGIN(KERNEL) : {
} >KERNEL

/* STM32F412 has to map the isr vector table at the very start of the internal
 * flash to boot correctly. */
.isr_vector_table : AT(ORIGIN(KERNEL) + SIZEOF(.signed_payload_prefix)) {
  _isr_vector_table_start_flash = LOADADDR(.isr_vector_table);
  _isr_vector_table_start_ram = .;
  KEEP(*(.isr_vector_table))
  _isr_vector_table_end_ram = .;
} >SRAM_DATA_BSS

.kernel_header ORIGIN(KERNEL) + SIZEOF(.isr_vector_table): {
  KEEP(*(.kernel_header))
} >KERNEL
