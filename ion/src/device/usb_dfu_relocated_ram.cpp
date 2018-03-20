#include <ion/usb.h>
#include "usb.h"
#include "device.h"
#include <stdlib.h>

extern char _dfu_bootloader_flash_start;
extern char _dfu_bootloader_flash_end;

namespace Ion {
namespace USB {

typedef void (*FunctionPointer)();

void DFU() {
  //TODO: Explain, in steps

  size_t dfu_bootloader_size = &_dfu_bootloader_flash_end - &_dfu_bootloader_flash_start;
  char * dfu_bootloader_ram_start = reinterpret_cast<char *>(0x20000000 + 256*1024 - 32*1024);

  char * ram_backup_buffer = static_cast<char *>(malloc(dfu_bootloader_size));

  if (ram_backup_buffer == nullptr) {
    return;
  }

  memcpy(ram_backup_buffer, dfu_bootloader_ram_start, dfu_bootloader_size);

  memcpy(dfu_bootloader_ram_start, &_dfu_bootloader_flash_start, dfu_bootloader_size);

  // Cortex-M expects jumps to be made to odd addresses when jumping to Thumb code
  // In general this is handled by the compiler, but here we're jumping to an arbitrary address
  // TODO: Check this is needed, maybe the compiler is super smart :)
  FunctionPointer dfu_bootloader_entry = reinterpret_cast<FunctionPointer>(dfu_bootloader_ram_start+1);

  dfu_bootloader_entry();

  memcpy(dfu_bootloader_ram_start, ram_backup_buffer, dfu_bootloader_size);

  free(ram_backup_buffer);
}

}
}
