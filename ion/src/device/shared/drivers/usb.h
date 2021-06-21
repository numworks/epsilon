#ifndef ION_DEVICE_SHARED_USB_H
#define ION_DEVICE_SHARED_USB_H

#include <ion/src/shared/platform_info.h>
#include <shared/drivers/board.h>
#include <shared/drivers/usb.h>
#include <shared/drivers/config/board.h>

#if 0
| INTERNAL FLASH             | OTP  |         EXTERNAL FLASH                                                                        |
| BOOTLOADER |  | Trampoline |      |                      SLOT A                                                                   |  SLOT B |
|                                   |          Signed Payload                |                                                      |
|                                   | Header | Arbitrary Data                       | Signature |                                   |
|                                   |        | Kernel                    | Userland |           |  FlashStorage | Persisting Bytes  |
|                                   |        | KH | Kernel data | 1-pad  | UH |     |


| Layout de la RAM                                         |
|

* The few restrictions to layout the RAM memory are the following:
 * - the begining of Kernel data/bss section should be aligned: the isr table
 * is within this section and the VTOR registers can only be configure with
 * some alignments constraints
 * - the kernel RAM should be aligned to be able to add MPU protection over
 * this region: MPU region must be aligned to the size of the region.
 * - Overflowing the userland stack should not impact the kernel (the MPU region
 * should protect it anyway)
 *
 * |                                SRAM                                                 |
 * | USERLAND DATA/BSS | HEAP | USERLAND STACK | KERNEL STACK | KERNEL DATA/BSS - canary |
 *
 * Bootloader SRAM is mapped like:
 * |                                    STACK                                   | canary |
 * Kernel SRAM is mapped like:
 * | xxxxxxxxxxxxxxxxxxxxxxxx | USERLAND STACK | KERNEL STACK | KERNEL DATA/BSS - canary |
 * Userland SRAM is mapped like:
 * | USERLAND DATA/BSS | HEAP | USERLAND STACK | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx |
 *
 */

Contraintes :
 - ISR en RAM ???? --> ne peut être layouté que sur la mémoire mappée par le cortex

class SignedPayloadHeader {
  uint32_t securityLevel;
  uint32_t size;
// By convention, we jump right at the address pointed by beginning of arbitrary data
};
// On the bootloader side, enforce entrypoint within [origin, origin+size];

class KernelHeader {
  FunctionPointer entryPoint;
  const char * kernel_version;
  const char * kernel_patch_level;
  uint32_t kernel_size; // userland address is infered from kernel_size
  ISR isr[30];
};

// Kernel rule for "can I write to flash?"
 -> For current slot, only in FlashStorage area
 -> For other slot, yes, anywhere

class UserlandHeader {
  void * externalAppsStart;
  void * externalAppsEnd;
  // storageInfo -> ramStorageInfo
  // flashStorageInfo
Userland:
-expected bootloader security level,
-expected os version 1.1.1
-expected os patch level
 - ex
-storage info?
- offset to external apps





Comportement du bootloader:
 - Regarder SLOT i

#endif

namespace Ion {
namespace Device {
namespace USB {

/* When entering DFU mode, we update this SlotInfo with a pointer to the current
 * PlatformInfo of the binary being run. The slot info is located at the very
 * beginning of the sRAM. */

class SlotInfo {
public:
  SlotInfo() :
    m_header(Magic),
    m_footer(Magic)
  {
  }
  void update() {
  m_platformInfoAddress =  reinterpret_cast<PlatformInfo *>(Board::userlandStart() + Ion::Device::Board::Config::UserlandHeaderOffset);
  }
  PlatformInfo * platformInfo() { return reinterpret_cast<PlatformInfo *>(m_platformInfoAddress); }
private:
  constexpr static uint32_t Magic = 0xEFEEDBBA;
  uint32_t m_header;
  PlatformInfo * m_platformInfoAddress;
  uint32_t m_footer;
};

SlotInfo * slotInfo();

void willExecuteDFU();
void didExecuteDFU();
bool shouldInterruptDFU();

}
}
}

#endif
