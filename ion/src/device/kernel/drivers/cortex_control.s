.syntax unified

.section .text.switch_to_unpriviledged
.align 2
.thumb
.global switch_to_unpriviledged
switch_to_unpriviledged:
  ldr r0, =_process_stack_start
  msr psp, r0
  mrs r0, control
  /* The first bit defined the unprivileged mode, the second bit indicates to
   * use the Process Stack Pointer */
  orrs r0, r0, #0x3
  msr control, r0
  isb 0xF
  bx  lr
.type switch_to_unpriviledged, function
