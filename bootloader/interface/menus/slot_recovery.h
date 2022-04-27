#ifndef _BOOTLOADER_INTERFACE_MENU_SLOT_RECOVERY_H
#define _BOOTLOADER_INTERFACE_MENU_SLOT_RECOVERY_H

#include <bootloader/interface/src/menu.h>
#include <bootloader/usb_data.h>

namespace Bootloader {
  class SlotRecoveryMenu : public Menu {
    public:
      SlotRecoveryMenu(USBData * usbData);

      void setup() override;
      void postOpen() override;
    private:
      const USBData * m_data;
  };
}

#endif
