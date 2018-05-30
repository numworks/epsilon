.syntax unified

.section .text
.global _entry
_entry:
  // Set up stack
  ldr sp, =_stack_start
  // Jump into epsilon
  b start
