#include "framebuffer.h"
#include <ion/display.h>
#include "main.h"

static KDColor sPixels[Ion::Display::Width * Ion::Display::Height];
static bool sFrameBufferActive = true;

namespace Ion {
namespace Display {

static KDFrameBuffer sFrameBuffer = KDFrameBuffer(sPixels, KDSize(Ion::Display::Width, Ion::Display::Height));

void pushRect(KDRect r, const KDColor * pixels) {
  if (sFrameBufferActive) {
    Simulator::Main::setNeedsRefresh();
    sFrameBuffer.pushRect(r, pixels);
  }
}

void pushRectUniform(KDRect r, KDColor c) {
  if (sFrameBufferActive) {
    Simulator::Main::setNeedsRefresh();
    sFrameBuffer.pushRectUniform(r, c);
  }
}

void pullRect(KDRect r, KDColor * pixels) {
  if (sFrameBufferActive) {
    sFrameBuffer.pullRect(r, pixels);
  }
}

}
}

namespace Ion {
namespace Simulator {
namespace Framebuffer {

const KDColor * address() {
  return sPixels;
}

void setActive(bool enabled) {
  sFrameBufferActive = enabled;
}

}
}
}
