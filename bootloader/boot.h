#ifndef BOOTLOADER_BOOT_H
#define BOOTLOADER_BOOT_H

#include <stdint.h>
#include <bootloader/slot.h>

namespace Bootloader {

enum BootMode: uint8_t {
  SlotA = 0,
  SlotB = 1,
  // These modes exists so that you can launch the bootloader from a running slot
  // They mean "Launch bootloader then go back to slot X"
  SlotABootloader = 2,
  SlotBBootloader = 3,
  Unknown
};

class Boot {
public:
  static BootMode mode();
  static void setMode(BootMode mode);
  __attribute__ ((noreturn)) static void boot();
  static void bootloader();
  static void aboutMenu();
  static void installerMenu();
  static void blupdate();
  static void bootSlot(Bootloader::Slot slot);
  static void lockInternal();
};

}

#endif