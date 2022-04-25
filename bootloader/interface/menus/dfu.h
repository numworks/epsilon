#ifndef _BOOTLOADER_INTERFACE_MENUS_DFU_H_
#define _BOOTLOADER_INTERFACE_MENUS_DFU_H_

#include <bootloader/interface/src/menu.h>
#include <bootloader/usb_data.h>

namespace Bootloader {
  class DfuMenu : public Menu {
    public:
      DfuMenu(const char * submenu, const USBData * usbData);

      void setup() override;
      void post_open() override;

    private:
      const char * m_submenu_text;
      const USBData * m_data;
  };
}

#endif