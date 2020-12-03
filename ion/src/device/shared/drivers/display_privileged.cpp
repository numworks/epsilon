#include "display_privileged.h"
#include "display.h"

#define USE_DMA_FOR_PUSH_PIXELS 0
#define USE_DMA_FOR_PUSH_COLOR 0

#define USE_DMA (USE_DMA_FOR_PUSH_PIXELS|USE_DMA_FOR_PUSH_COLOR)

namespace Ion {
namespace Display {

using namespace Device::Display;

void pushRectUniform(KDRect r, KDColor c) {
  // Store r and c
  setTempKD(&r);
  setTempC(c);
  pushRectUniformSVC();
  Ion::Timing::msleep(100);
  stampB();
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

}
}
}
