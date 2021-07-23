.syntax unified

.section .text.switch_to_unpriviledged
.align 2
.thumb
.global switch_to_unpriviledged
/* TODO: rename function?
 * n0100 never switches to unprivileged to be able to implement the DFU leave:
 * we want to jump and execute code as if we reset (without using a core reset
 * that might end up on ST bootloader if the device is plugged). Therefore,
 * from the DFU code, which is executed in thread mode, we want to be able to
 * overwrite the CONTROL register which requires a privileged access. While
 * running the DFU driver, we can't access any interruption since the internal
 * flash might be corrupted. */
switch_to_unpriviledged:
  // Initialize the process sp
  ldr r0, =_process_stack_start
  msr psp, r0
  ldr r0, =_main_stack_start
  msr msp, r0
  // Reset the main sp
  mrs r0, control
  /* The least significant bit defined the unprivileged mode, the next bit
   * indicates to use the Process Stack Pointer. We stay in privileged mode but
   * use the PSP in thread mode. */
  orrs r0, r0, #0x2
  msr control, r0
  isb 0xF
  bx  lr
.type switch_to_unpriviledged, function
