.syntax unified

.section .text
.align 2
.thumb
.global setjmp
setjmp:
  /* Save all the registers into the jump buffer */
  mov ip, sp
  // storing r4-r9, r10=sl, r11=fp, ip=sp, lr
  stmia r0!, { r4-r11, ip, lr}
  // storing floating point registers
  vstmia r0!, {s16-s31}
  // Special storing: fpscr
  vmrs r1, fpscr
  str  r1, [r0, #4]
  /* And then return 0 */
  mov  a1, #0
  bx lr
.type setjmp, function
