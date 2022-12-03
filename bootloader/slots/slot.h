#ifndef BOOTLOADER_SLOTS_SLOT_H
#define BOOTLOADER_SLOTS_SLOT_H

#include <stdint.h>

#include "kernel_header.h"
#include "userland_header.h"

namespace Bootloader {

class Slot {

public:
  Slot(uint32_t address) :
    m_kernelHeader(reinterpret_cast<KernelHeader*>(address)),
    m_userlandHeader(reinterpret_cast<UserlandHeader*>(address + 64 * 1024)),
    m_userland2Header(reinterpret_cast<UserlandHeader*>(address + 128 * 1024)),
    m_address(address) {}

  const KernelHeader* kernelHeader() const;
  const UserlandHeader* userlandHeader() const;
  [[ noreturn ]] void boot() const;
  const uint32_t address() const { return m_address; }

  static const Slot A();
  static const Slot B();
  static const Slot Khi();

  static bool isFullyValid(const Slot& slot) {
    return slot.kernelHeader()->isValid() && slot.userlandHeader()->isValid();
  }

private:
  const KernelHeader* m_kernelHeader;
  const UserlandHeader* m_userlandHeader;
  const UserlandHeader* m_userland2Header;
  const uint32_t m_address;

};

}

#endif