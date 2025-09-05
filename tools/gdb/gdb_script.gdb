define write_elf
  monitor reset init
  monitor flash write_image erase $arg0
  exit
end

# start device connection

target extended-remote localhost:3333
set pagination off
