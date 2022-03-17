#ifndef BOOTLOADER_SLOT_H
#define BOOTLOADER_SLOT_H

#include <stdint.h>

#include "kernel_header.h"
#include "userland_header.h"

namespace Bootloader {

class Slot {

public:
  Slot(uint32_t address) :
    m_kernelHeader(reinterpret_cast<KernelHeader*>(address)),
    m_userlandHeader(reinterpret_cast<UserlandHeader*>(address + 64 * 1024)) { }

  const KernelHeader* kernelHeader() const;
  const UserlandHeader* userlandHeader() const;
  [[ noreturn ]] void boot() const;

  static const Slot A();
  static const Slot B();

private:
  const KernelHeader* m_kernelHeader;
  const UserlandHeader* m_userlandHeader;

};

}

#endif