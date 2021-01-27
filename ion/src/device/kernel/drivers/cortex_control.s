.syntax unified

.section .text.switch_to_unpriviledged
.align 2
.thumb
.global switch_to_unpriviledged
switch_to_unpriviledged:
  mov r0, #0x01
  msr control, r0
  bx  lr
.type switch_to_unpriviledged, function
