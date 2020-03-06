# We want to test a bunch of addresses and sizes

import os

def random_write(flash_offset, length):
  flash_base_address = 0x08000000
  os.system("head -c %d /dev/urandom > data.bin" % length)
  os.system("dfu-util -i 0 -a 0 -s 0x%x -D data.bin > /dev/null 2>&1" % (flash_base_address + flash_offset))
  os.system("rm result.bin")
  os.system("dfu-util -i 0 -a 0 -s 0x%x:%d -U result.bin > /dev/null 2>&1" % (flash_base_address, flash_offset + length + 100))
  data = open('data.bin', 'rb').read()
  result = open('result.bin', 'rb').read()
  theoretical_result = "\xFF"*flash_offset + data + "\xFF"*100
  return (result == theoretical_result)

pairs = [(0,1), (5,2), (37,33), (211,2719), (0x4000-5, 17)]
# The last test crosses sector boundaries
for (offset,length) in pairs:
  if not random_write(offset, length):
    print("[ERROR] Write %d bytes at offset 0x%x" % (length, offset))
  else:
    print("[OK] Write %d bytes at offset 0x%x" % (length, offset))
