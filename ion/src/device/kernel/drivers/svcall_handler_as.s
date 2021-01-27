.syntax unified

.section .text.svcall_handler_as
.align 2
.thumb
.global svcall_handler_as
svcall_handler_as:
  ldr r0,[sp,#0x18]
  ldrh r0,[r0,#-2]
  bic r0,r0,#0xFF00
  mov r1,sp
  push {lr}
  bl svcall_handler
  ldmia.w sp!,{lr}
  bx lr
.type svcall_handler_as, function
