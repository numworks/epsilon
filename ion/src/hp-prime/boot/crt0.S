.syntax unified

.section .text
.global _entry
_entry:
  // Disable interrupts right away
  ldr r0, =0xffffffff
  ldr r1, =0x4a000000
  str r0, [r1, #0x8]
  str r0, [r1, #0x48]

  // Set up stack
  ldr sp, =_stack_start

  // Jump into epsilon
  b start
