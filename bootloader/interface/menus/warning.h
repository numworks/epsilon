#ifndef _BOOTLOADER_INTERFACE_MENUS_WARNING_H_
#define _BOOTLOADER_INTERFACE_MENUS_WARNING_H_

#include <bootloader/interface/src/menu.h>
#include <bootloader/slots/slot.h>

namespace Bootloader {
  class WarningMenu : public Menu {
    public:
      WarningMenu();

      void setup() override;
      void postOpen() override {};
  };
}

#endif
