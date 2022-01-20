.syntax unified

.section .text
.align 2
.thumb
.global longjmp
longjmp:
  /* Restore all the registers to get back in the original state (whenever the
     matching setjmp was called. */
  // General purpose registers
  ldmia  r0!, { r4-r11, ip, lr }
  // Floating point registers
  vldmia r0!, { s16-s31 }
  // Special case for VFP status register
  ldr r0, [r0, #4]
  vmsr fpscr, r0
  mov  sp, ip
  /* Put the return value in the integer result register, but return 1 if it is
     in fact zero. */
  movs r0, r1
  it eq
  moveq r0, #1
  bx lr
.type longjmp, function
