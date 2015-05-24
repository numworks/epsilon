# Let's connect to OpenOCD
target remote localhost:3333

# GDB pagniation is annoying
set pagination off

# Load our executable
load boot.elf

# Tell OpenOCD to reset and halt
monitor reset halt

break init
break abort
break __assert

continue
