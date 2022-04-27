#ifndef _BOOTLOADER_INTERFACE_ABOUT_H_
#define _BOOTLOADER_INTERFACE_ABOUT_H_

#include <bootloader/interface/src/menu.h>

namespace Bootloader {
  class AboutMenu : public Menu {
    public:
      AboutMenu();
    
      void setup() override;
      void postOpen() override {};
  };
}


#endif
