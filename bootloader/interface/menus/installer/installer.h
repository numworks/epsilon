#ifndef _BOOTLOADER_INTERFACE_MENUS_INSTALLER_INSTALLER_H_
#define _BOOTLOADER_INTERFACE_MENUS_INSTALLER_INSTALLER_H_

#include <bootloader/interface/src/menu.h>

namespace Bootloader {
  class InstallerMenu : public Menu {
    public:
      InstallerMenu();

      void setup() override;
  };
}

#endif