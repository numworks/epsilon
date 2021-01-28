.syntax unified

.section .text.svcall_handler_as
.align 2
.thumb
.global svcall_handler_as
svcall_handler_as:
  // Store r0,r1,lr (TODO: r0/r1 are scratch registers, do we need to preserve them?)
  push {r0,r1,lr}
  // STEP 1: extract the SVC_NUMBER
  /* 1.1 Load the return value in r0 (to access the instruction "svc SVC_NUMBER")
   * NB: the return value is not $lr because we're within an interruption: $lr
   * was push in the context frame stored on the stack. $lr offset is 0x18 but
   * since we pushed 3 registers before, we need to read with an offset of 0x24
   * (=0x18+OxC)
   */
  ldr r0,[sp,#0x24]
  // 1.2 Load the instruction "svc SVC_NUMBER" in r0
  ldrh r0,[r0,#-2]
  // 1.3 Extract SVC_NUMBER in r0
  bic r0,r0,#0xFF00
  /* STEP 2: extract the arguments to be passed to svcall_handler
   * 2.1 Get the arguments pointers in r1 (which is on the stack since we disable
   * any optimization on SVC wrapping function). */
  mov r1,sp
  // 2.2 Don't forget we pushed 3 registers on the stack in the prologue
  add r1,#0xC
  // STEP 3: jump to svcall_handler with r0 = SVC_NUMBER and r1 = args pointer
  bl svcall_handler
  // Restore register
  ldmia.w sp!,{r0,r1,lr}
  // Return from exception
  bx lr
.type svcall_handler_as, function
