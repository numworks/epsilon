#ifndef _BOOTLOADER_INTERFACE_MENUS_HOME_H_
#define _BOOTLOADER_INTERFACE_MENUS_HOME_H_

#include <bootloader/interface/src/menu.h>
#include <bootloader/interface/menus/about.h>
#include <bootloader/interface/menus/installer.h>

namespace Bootloader {
  class HomeMenu : public Menu {
    public:
      HomeMenu();

      void setup() override;
      void post_open() override {};

      static AboutMenu * aboutMenu();
      static InstallerMenu * installerMenu();

    private:
      const char * slotA_text();
      const char * slotA_kernel_text();
      const char * slotA_os_text();
      const char * slotA_version_text();
      const char * slotKhi_text();
      const char * slotKhi_kernel_text();
      const char * slotKhi_os_text();
      const char * slotKhi_version_text();
      const char * slotB_text();   
      const char * slotB_kernel_text();
      const char * slotB_os_text();
      const char * slotB_version_text();
      
  };
}

#endif
