.syntax unified

.section .text
.align 2
.thumb
.global longjmp
longjmp:
  /* Restore all the regsiters to get back in the original state (whenever the
     matching setjmp was called. */
  ldmia r0!, { r4-r7 }
  ldmia r0!, { r2-r3 }
  mov r8, r2
  mov r9, r3
  ldmia r0!, { r2-r3 }
  mov r10, r2
  mov fp, r3
  ldmia r0!, { r2-r3 }
  mov sp, r2
  mov lr, r3
  /* Put the return value in the integer result register, but return 1 if it is
     in fact zero. */
  movs r0, r1
  cmp r0, #0
  bne _ret
  movs r0, #1
_ret:
  bx lr
.type longjmp, function
