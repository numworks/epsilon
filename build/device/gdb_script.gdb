# Add a routine to debug ARM exceptions

define armex
  printf "SCB_CFSR        0x%x\n", *0xE000ED28
  printf "SCB_HFSR        0x%x\n", *0xE000ED2C
  printf "SCB_MMAR        0x%x\n", *0xE000ED34
  printf "SCB_BFAR        0x%x\n", *0xE000ED38
  printf "xPSR            0x%x\n", *(int *)($msp+28)
  printf "ReturnAddress   0x%x\n", *(int *)($msp+24)
  printf "LR (R14)        0x%x\n", *(int *)($msp+20)
  printf "R12             0x%x\n", *(int *)($msp+16)
  printf "R3              0x%x\n", *(int *)($msp+12)
  printf "R2              0x%x\n", *(int *)($msp+8)
  printf "R1              0x%x\n", *(int *)($msp+4)
  printf "R0              0x%x\n", *(int *)($msp)
  printf "Return instruction:\n"
  x/i *(int *)($msp+24)
end

define load_isr
  set $sp = *(InitialisationVector)
# Warning: InitialisationVector is a uint32_t*, so InitialisationVector+1 points to the next 32-bit value
  set $pc = *(InitialisationVector+1)
end


define use_dfu_symbol_file
# Discard previous symbol file
  symbol-file
# Load new symbol file
  add-symbol-file build/debug/device/ion/src/device/shared/usb/dfu.elf 0x20038000
end

document armex
ARMv7 Exception entry behavior.
xPSR, ReturnAddress, LR (R14), R12, R3, R2, R1, and R0
end

# Let's connect to OpenOCD
target remote localhost:3333

# GDB pagniation is annoying
set pagination off

# Load our executable
load

# Tell OpenOCD to reset and halt
# monitor itm ports on
# monitor tpiu config internal swo.log.bin uart off 16000000
# monitor reset halt

# continue
