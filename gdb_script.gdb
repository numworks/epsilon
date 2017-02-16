# Let's connect to OpenOCD
target remote localhost:3333

# GDB pagniation is annoying
set pagination off

# Load our executable
load

# Tell OpenOCD to reset and halt
monitor itm ports on
monitor tpiu config internal swo.log.bin uart off 16000000
monitor reset halt

break init
break abort
break __assert

continue
