#ifndef BOOTLOADER_INTERFACE_STATIC_INTERFACE_H
#define BOOTLOADER_INTERFACE_STATIC_INTERFACE_H

#include <stdint.h>
#include <kandinsky/context.h>
#include <escher/image.h>

namespace Bootloader {
class Interface {

public:
  static void drawImage(KDContext * ctx, const Image * image, int offset);
  static void drawLoading();
  static void drawFlasher();
};

}

#endif