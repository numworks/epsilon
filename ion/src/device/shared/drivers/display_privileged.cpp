#include "display_privileged.h"
#include "display.h"
#include "svcall.h"
#include <kandinsky/rect.h>
#include <kandinsky/color.h>
extern "C" {
#include <stddef.h>
}

#define USE_DMA_FOR_PUSH_PIXELS 0
#define USE_DMA_FOR_PUSH_COLOR 0

#define USE_DMA (USE_DMA_FOR_PUSH_PIXELS|USE_DMA_FOR_PUSH_COLOR)

namespace Ion {
namespace Display {

using namespace Device::Display;

// TODO HUGO : Factorize pushRect and pushRectSVC()

void pushRect(KDRect r, const KDColor * pixels) {
#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  setDrawingArea(r, Orientation::Landscape);
  pushPixels(pixels, r.width() * r.height());
}

void pushRectUniform(KDRect r, KDColor c) {
#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  setDrawingArea(r, Orientation::Portrait);
  pushColor(c, r.width() * r.height());
}

void pullRect(KDRect r, KDColor * pixels) {
#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  setDrawingArea(r, Orientation::Landscape);
  pullPixels(pixels, r.width()*r.height());
}

}
}


namespace Ion {
namespace Device {
namespace Display {

using namespace Device::Display;

void pushRectSVC() {
  // Load rect and pixels
  void * args[2];
  getSvcallArgs(2, args);
  if (args[0] == nullptr || args[1] == nullptr) {
    return;
  }
  KDRect r = *static_cast<KDRect *>(args[0]);
  const KDColor * pixels = *static_cast<const KDColor **>(args[1]);

#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  setDrawingArea(r, Orientation::Landscape);
  pushPixels(pixels, r.width() * r.height());
}

void pushRectUniformSVC() {
  // Load rect and color
  void * args[2];
  getSvcallArgs(2, args);
  if (args[0] == nullptr || args[1] == nullptr) {
    return;
  }
  KDRect r = *static_cast<KDRect *>(args[0]);
  KDColor c = *static_cast<KDColor *>(args[1]);

#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  setDrawingArea(r, Orientation::Portrait);
  pushColor(c, r.width() * r.height());
}

void pullRectSVC() {
  // Load rect and pixels
  void * args[2];
  getSvcallArgs(2, args);
  if (args[0] == nullptr || args[1] == nullptr) {
    return;
  }
  KDRect r = *static_cast<KDRect *>(args[0]);
  KDColor * pixels = *static_cast<KDColor **>(args[1]);

#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  setDrawingArea(r, Orientation::Landscape);
  pullPixels(pixels, r.width()*r.height());
}

}
}
}
