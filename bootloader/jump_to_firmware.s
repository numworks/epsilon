
.syntax unified
.section .text.jump_to_firmware
.align 2
.thumb
.global jump_to_firmware
jump_to_firmware:
  dsb 0xF
  isb 0xF
  msr msp, r0
  bx r1
