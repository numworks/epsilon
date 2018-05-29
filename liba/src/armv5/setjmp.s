.syntax unified

.section .text
.global setjmp
setjmp:
  /* Save all the registers into the jump buffer */
  mov ip, sp
  // storing r4-r9, r10=sl, r11=fp, ip=sp, lr
  stmia r0!, { r4-r11, ip, lr}

  // XXX: we don't store VFP state

  /* And then return 0 */
  mov  a1, #0
  bx lr
.type setjmp, function
