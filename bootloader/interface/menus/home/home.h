#ifndef _BOOTLOADER_INTERFACE_HOME_HOME_H_
#define _BOOTLOADER_INTERFACE_HOME_HOME_H_

#include <bootloader/interface/src/menu.h>
#include <bootloader/interface/menus/about/about.h>

namespace Bootloader {
  class HomeMenu : public Menu {
    public:
      HomeMenu();

      void setup() override;
      static AboutMenu * aboutMenu();

    private:
      const char * slotA_text();
      const char * slotKhi_text();
      const char * slotB_text();   
      
  };
}

#endif
