#include "display.h"
#include "svcall.h"
#include "svcall_args.h"

namespace Ion {
namespace Display {

using namespace Device::Display;

/* We isolate the standby code that needs to be executed from the internal
 * flash (because the external flash is then shut down). We forbid inlining to
 * avoid inlining these instructions in the external flash. */

void pushRect(KDRect r, const KDColor * pixels) {
  // Store rect and pixels
  const char * args[2] = {(char *)&r, (char *)&pixels};
  svcall(SVC_PUSH_RECT, 2, args);
}

void pushRectUniform(KDRect r, KDColor c) {
  // Store rect and color
  const char * args[2] = {(char *)&r, (char *)&c};
  svcall(SVC_PUSH_RECT_UNIFORM, 2, args);
}

}
}
