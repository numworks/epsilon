.syntax unified

.section .text
.align 2
.thumb
.global longjmp
longjmp:
  /* Restore all the regsiters to get back in the original state (whenever the
     matching setjmp was called. */
  bkpt #0 // TODO cf armv7m implementation
  bx lr
.type longjmp, function
