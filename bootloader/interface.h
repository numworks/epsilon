#ifndef BOOTLOADER_INTERFACE
#define BOOTLOADER_INTERFACE

#include <stdint.h>
#include <kandinsky/context.h>
#include <escher/image.h>

namespace Bootloader {

class Interface {

private:
  static void drawImage(KDContext* ctx, const Image* image, int offset);

public:
  static void draw();

};

}

#endif