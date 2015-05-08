# Let's connect to OpenOCD
target remote localhost:3333

# Load our executable
load boot.elf

# Tell OpenOCD to reset and halt
monitor reset halt

break init
break _halt

continue
