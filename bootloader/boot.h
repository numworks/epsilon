#ifndef BOOTLOADER_BOOT_H
#define BOOTLOADER_BOOT_H

#include <stdint.h>
#include <bootloader/slots/slot.h>

namespace Bootloader {

class BootConfig {
public:
    BootConfig() : m_slot(nullptr), m_booting(false) {};

    void setSlot(Slot * slot) { m_slot = slot; }
    Slot * slot() const { return m_slot; }
    void clearSlot() { m_slot = nullptr; }

    void setBooting(bool booting) { m_booting = booting; }
    bool isBooting() const { return m_booting; }
private:
    Bootloader::Slot * m_slot;
    bool m_booting;
};

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
  static BootConfig * config();

  static bool isKernelPatched(const Slot & slot);
  static void patchKernel(const Slot & slot);

  static void busError();

  __attribute__ ((noreturn)) static void boot();

  static void bootSlot(Bootloader::Slot slot);
  static void bootSelectedSlot();
  __attribute__ ((noreturn)) static void jumpToInternalBootloader(); 
  __attribute((section(".fake_isr_function"))) __attribute__((used)) static void flash_interrupt();

  static void bootloader();
  static void lockInternal();
  static void EnableInternalFlashInterrupt();
};


}

#endif
