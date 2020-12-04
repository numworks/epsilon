#include "display_privileged.h"
#include "display.h"
#include <drivers/svcall_args.h>

#define USE_DMA_FOR_PUSH_PIXELS 0
#define USE_DMA_FOR_PUSH_COLOR 0

#define USE_DMA (USE_DMA_FOR_PUSH_PIXELS|USE_DMA_FOR_PUSH_COLOR)

namespace Ion {
namespace Display {

using namespace Device::Display;

void pushRectUniform(KDRect r, KDColor c) {
  // Store r and c
  const char * args[2] = {(char *)&r, (char *)&c};
  svcArgs(2, args);
  pushRectUniformSVC();
}

void pushRect(KDRect r, const KDColor * pixels) {
  // Store r and c
  const char * args[2] = {(char *)&r, (char *)&pixels};
  svcArgs(2, args);
  pushRectSVC();
}

}
}


namespace Ion {
namespace Device {
namespace Display {

using namespace Device::Display;

void pushRectUniformSVC() {
#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  pushRU();
}

void pushRectSVC() {
#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  pushR();
}

}
}
}
