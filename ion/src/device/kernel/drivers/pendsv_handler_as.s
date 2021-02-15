.syntax unified

.section .text.pendsv_handler_as
.align 2
.thumb
.global pendsv_handler_as
pendsv_handler_as:
  // Store r0,r1
  push {r0,r1}
  /* STEP 1: extract the stack frame address
   * 1.1 Extract the address of the context frame. */
  mov r0,sp
  // 1.2 Don't forget we pushed some registers to the stack in the prologue
  add r0,0x8
  // STEP 2: Extract the exception return value
  mov r1,lr
  // Store lr
  push {lr}
  /* STEP 3: jump to pendsv_handler with:
   * - r0 = frame address
   * - r1 = exception return */
  bl pendsv_handler
  // Restore registers
  pop {lr}
  pop {r0,r1}
  // Return from exception
  bx lr
.type pendsv_handler_as, function

