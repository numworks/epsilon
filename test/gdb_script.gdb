# Let's connect to OpenOCD
target remote localhost:3333

# Load our executable
load test.elf

# Tell OpenOCD to reset and halt
monitor reset halt

# Add a breakpoint
break debugger

# Launch the test suite
continue
