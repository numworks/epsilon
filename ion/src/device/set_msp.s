.syntax unified

.section .text
.align 2
.thumb
.global set_msp
set_msp:
  msr msp, r0
  bx lr
.type set_msp, function
