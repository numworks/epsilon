#include <ion/display.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor * pixels) {
  // Store rect and pixels
  void * args[2] = { static_cast<void *>(&r), static_cast<void *>(&pixels) };
  int argc = sizeof(args)/sizeof(void *);
  Ion::Device::SVCall::svcall(SVC_PUSH_RECT, &argc, args);
}

void pushRectUniform(KDRect r, KDColor c) {
  // Store rect and color
  void * args[2] = { static_cast<void *>(&r), static_cast<void *>(&c) };
  int argc = sizeof(args)/sizeof(void *);
  Ion::Device::SVCall::svcall(SVC_PUSH_RECT_UNIFORM, &argc, args);
}

void pullRect(KDRect r, KDColor * pixels) {
  // Store rect and pixels
  void * args[2] = { static_cast<void *>(&r), static_cast<void *>(&pixels) };
  int argc = sizeof(args)/sizeof(void *);
  Ion::Device::SVCall::svcall(SVC_PULL_RECT, &argc, args);
}

void POSTPushMulticolor(int rootNumberTiles, int tileSize) {
  // Store rootNumberTiles and tileSize
  void * args[2] = { static_cast<void *>(&rootNumberTiles), static_cast<void *>(&tileSize) };
  int argc = sizeof(args)/sizeof(void *);
  Ion::Device::SVCall::svcall(SVC_POST_PUSH_MULTICOLOR, &argc, args);
}

}
}
