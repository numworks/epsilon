#include "display.h"
#include <drivers/svcall.h>

namespace Ion {
namespace Display {

using namespace Device::Display;

/* We isolate the standby code that needs to be executed from the internal
 * flash (because the external flash is then shut down). We forbid inlining to
 * avoid inlining these instructions in the external flash. */

void pushRectUniform(KDRect r, KDColor c) {
  // Store r and c
  setTempKD(&r);
  setTempC(c);
  svc(SVC_PUSH_RECT_UNIFORM);
  Ion::Timing::msleep(100);
  stampC();
}


}
}
