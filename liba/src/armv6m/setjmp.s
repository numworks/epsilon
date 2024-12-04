.syntax unified

.section .text
.align 2
.thumb
.global setjmp
setjmp:
  /* Save all the registers into the jump buffer */
  bkpt #0 // TODO cf armv7m implementation
  bx lr
.type setjmp, function
