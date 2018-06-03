.syntax unified

.section .header
.word _entry                                // entry
.word 0                                     // padding
.word 0x100000                              // size ?
.word 0x30000000                            // load addr
.word 0x100000                              // size ?
.string "V5J"                               // cpu family?
soc_str:
.string "2416"                              // soc family?
.zero (8+soc_str-.)

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
