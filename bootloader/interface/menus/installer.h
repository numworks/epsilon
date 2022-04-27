#ifndef _BOOTLOADER_INTERFACE_MENUS_INSTALLER_H_
#define _BOOTLOADER_INTERFACE_MENUS_INSTALLER_H_

#include <bootloader/interface/src/menu.h>
#include <bootloader/interface/menus/dfu.h>

namespace Bootloader {
  class InstallerMenu : public Menu {
    public:
      InstallerMenu();

      void setup() override;
      void postOpen() override {};

      static DfuMenu * SlotsDFU();
      static DfuMenu * BootloaderDFU();
  };
}

#endif
