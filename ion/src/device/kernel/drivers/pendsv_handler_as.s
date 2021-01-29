.syntax unified

.section .text.pendsv_handler_as
.align 2
.thumb
.global pendsv_handler_as
pendsv_handler_as:
  // Store r0,r1 (TODO: r0/r1 are scratch registers, do we need to preserve them?)
  push {r0,r1}
  // Extract the address of the context frame
  mov r0,sp
  // Don't forget we pushed some registers to the stack in the prologue
  add r0,0x8
  // Extract the exception return value in
  mov r1,lr
  // Store lr
  push {lr}
  // Call svcall_handler with r0 = frame address and r1 = exception return
  bl pendsv_handler
  // Restore registers
  pop {lr}
  pop {r0,r1}
  // Return from exception
  bx lr
.type pendsv_handler_as, function

