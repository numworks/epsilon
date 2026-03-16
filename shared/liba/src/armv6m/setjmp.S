.syntax unified

.section .text
.align 2
.thumb
.global setjmp
setjmp:
  /* Save all the registers into the jump buffer */
  mov ip, sp
  // storing r4-r9, r10=sl, fp, sp, lr
  stmia r0!, { r4-r7 }
  // since we cannot use stmia with regs after r7 we do it in three steps
  mov r1, r8
  mov r2, r9
  mov r3, r10
  stmia r0!, { r1-r3 }
  mov r1, fp
  mov r2, sp
  mov r3, lr
  stmia r0!, { r1-r3 }
  /* And then return 0 */
  movs r0, #0
  bx lr
.type setjmp, function
