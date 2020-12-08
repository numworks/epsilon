#include "display.h"
#include "svcall.h"

namespace Ion {
namespace Display {

using namespace Device::Display;

/* We isolate the standby code that needs to be executed from the internal
 * flash (because the external flash is then shut down). We forbid inlining to
 * avoid inlining these instructions in the external flash. */

void pushRect(KDRect r, const KDColor * pixels) {
  // Store rect and pixels
  void * args[2] = { static_cast<void *>(&r), static_cast<void *>(&pixels) };
  svcall(SVC_PUSH_RECT, 2, args);
}

void pushRectUniform(KDRect r, KDColor c) {
  // Store rect and color
  void * args[2] = { static_cast<void *>(&r), static_cast<void *>(&c) };
  svcall(SVC_PUSH_RECT_UNIFORM, 2, args);
}

void pullRect(KDRect r, KDColor * pixels) {
  // Store rect and pixels
  void * args[2] = { static_cast<void *>(&r), static_cast<void *>(&pixels) };
  svcall(SVC_PULL_RECT, 2, args);
}

}
}
