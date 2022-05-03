#ifndef BOOTLOADER_INTERFACE_STATIC_INTERFACE_H
#define BOOTLOADER_INTERFACE_STATIC_INTERFACE_H

#include <stdint.h>
#include <kandinsky/context.h>
#include <escher/image.h>

namespace Bootloader {
class Interface {

public:
  static void drawComputer(KDContext * ctx, int offset);
  static KDCoordinate computerHeight();
  static void drawLoading();
  static void drawFlasher();
};

}

#endif
