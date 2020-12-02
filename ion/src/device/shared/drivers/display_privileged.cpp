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
  // register KDRect *r asm ("r2");
  // register KDColor *c asm ("r3");
  setTempKD(&r);
  setTempC(c);
  pushRectUniformSVC();
  Ion::Timing::msleep(100);
  stampB();
  // svc(SVC_PUSH_RECT_UNIFORM);
  // pushRU();
  // (void)r;
  // (void)c;
}

}
}


namespace Ion {
namespace Device {
namespace Display {

using namespace Device::Display;

void pushRectSVC() {
  // Retrieve r and pixels
  // register KDRect *pR asm ("r2");
  // register (const KDColor *) *pPixels asm ("r3");
  // KDRect r =*pR;
  // const KDColor * pixels = *pPixels;
#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  pushR();
  // setDrawingArea(getTempKD(), Orientation::Landscape);
  // pushPixels(getTempPixels(), getTempKD().width()*getTempKD().height());
}

void pushRectUniformSVC() {
  // Retrieve r and c
  // register KDRect *pR asm ("r2");
  // register KDColor *pC asm ("r3");
  // KDRect r = *pR;
  // const KDColor c= *pC;
#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  pushRU();
  // setDrawingArea(getTempKD(), Orientation::Portrait);
  // pushColor(getTempC(), getTempKD().width()*getTempKD().height());
}

}
}
}
