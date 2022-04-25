#ifndef _BOOTLOADER_INTERFACE_MENUS_CRASH_H_
#define _BOOTLOADER_INTERFACE_MENUS_CRASH_H_

#include <bootloader/interface/src/menu.h>

namespace Bootloader {
  class CrashMenu : public Menu {
    public:
      CrashMenu(const char * error);

      void setup() override;
      void post_open() override;

    private:
      const char * m_error;
  };
}

#endif