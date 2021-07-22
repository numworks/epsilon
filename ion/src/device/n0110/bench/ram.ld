INCLUDE config_flash.ld

CONFIG_OFFSET = BENCH_OFFSET;
CONFIG_LENGTH = BENCH_LENGTH;

INCLUDE config_ram.ld

SECTIONS {
  .signed_payload_prefix ORIGIN(RAM_BUFFER) : {
    /* We keep two 4-byte words to store:
     * - the signed payload size
     * - the security level
     * These are completed at the signature time since we don't know the whole
     * payload sign at linking time. */
    LONG(0xFFFFFFFF)
    LONG(0xFFFFFFFF)
  } >RAM_BUFFER

  .isr_vector_table : AT(ORIGIN(RAM_BUFFER) + SIZEOF(.signed_payload_prefix)) {
    _isr_start = .;
    KEEP(*(.isr_vector_table))
  } >RAM_BUFFER

  .text : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >RAM_BUFFER

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >RAM_BUFFER

  .rodata : {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata.*)
  } >RAM_BUFFER

  .data : {
    . = ALIGN(4);
    *(.data)
    *(.data.*)
  } >RAM_BUFFER

  .bss : {
    . = ALIGN(4);
    *(.bss)
    *(.bss.*)
    *(COMMON)
  } >RAM_BUFFER

  .stack : {
    . = ALIGN(8);
    _stack_end = .;
    . += (STACK_SIZE - 8);
    . = ALIGN(8);
    _stack_start = .;
  } >RAM_BUFFER

  .signature : {
    FILL(0xFF);
    /* We write the last byte of the signature with a default value to make
     * sure the userland size takes into account the signature. */
    . += SIGNATURE_LENGTH - 1;
    BYTE(0xFF)
  } >RAM_BUFFER
}
