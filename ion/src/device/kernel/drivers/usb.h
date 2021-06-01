#ifndef ION_DEVICE_KERNEL_USB_H
#define ION_DEVICE_KERNEL_USB_H

#include <kernel/drivers/board.h>
#include <ion/src/shared/platform_info.h>
#include <shared/drivers/usb.h>
#include <shared/drivers/config/board.h>

namespace Ion {
namespace Device {
namespace USB {

/* We keep some information in the RAM regarding:
 * - which slot is running
 * - what versions was previously run (before being overriden in N0100 mode)
 * to be accessible via DFU. */

class SlotInfo {
public:
  SlotInfo() :
    m_header(Magic),
    m_footer(Magic)
  {
  }
  void update() {
    m_slotA = Board::isRunningSlotA();
    memcpy(&m_platformInfoBuffer, reinterpret_cast<PlatformInfo *>(Board::userlandStart() + Ion::Device::Board::Config::UserlandHeaderOffset), sizeof(PlatformInfo));
  }
  PlatformInfo * platformInfo() { return reinterpret_cast<PlatformInfo *>(m_platformInfoBuffer); }
private:
  constexpr static uint32_t Magic = 0xEFEEDBBA;
  uint32_t m_header;
  bool m_slotA;
  uint8_t m_platformInfoBuffer[sizeof(PlatformInfo)];
  uint32_t m_footer;
};

SlotInfo * slotInfo();

}
}
}

#endif
