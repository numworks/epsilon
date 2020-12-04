#include "display_privileged.h"
#include "display.h"
#include "svcall_args.h"
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

void pushRect(KDRect r, const KDColor * pixels) {
  // Store rect and pixels
  const char * args[2] = {(char *)&r, (char *)&pixels};
  setSvcallArgs(2, args);
  pushRectSVC();
}

void pushRectUniform(KDRect r, KDColor c) {
  // Store rect and color
  const char * args[2] = {(char *)&r, (char *)&c};
  setSvcallArgs(2, args);
  pushRectUniformSVC();
}

}
}


namespace Ion {
namespace Device {
namespace Display {

using namespace Device::Display;

void pushRectSVC() {
#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  // Load rect and pixels
  const char * args[2];
  getSvcallArgs(2, args);
  KDRect r = *(KDRect *)args[0];
  const KDColor * pixels = *(const KDColor **)args[1];

  setDrawingArea(r, Orientation::Landscape);
  pushPixels(pixels, r.width()*r.height());
}

void pushRectUniformSVC() {
#if USE_DMA
  waitForPendingDMAUploadCompletion();
#endif
  // Load rect and color
  const char * args[2];
  getSvcallArgs(2, args);
  KDRect r = *(KDRect *)args[0];
  KDColor c = *(KDColor *)args[1];

  setDrawingArea(r, Orientation::Portrait);
  pushColor(c, r.width()*r.height());
}

}
}
}
