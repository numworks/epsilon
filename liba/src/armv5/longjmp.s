.syntax unified

.section .text
.global longjmp
longjmp:
  /* Restore all the regsiters to get back in the original state (whenever the
     matching setjmp was called. */
  // General purpose registers
  ldmia  r0!, { r4-r11, ip, lr }

  mov  sp, ip
  /* Put the return value in the integer result register, but return 1 if it is
     in fact zero. */
  movs r0, r1
  it eq
  moveq r0, #1
  bx lr
.type longjmp, function
