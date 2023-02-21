#include "framebuffer.h"

#include <ion/display.h>
#include <kandinsky/color.h>
#include <kandinsky/framebuffer.h>

#include "window.h"

/* Drawing on an SDL texture
 * In SDL2, drawing bitmap data happens through textures, whose data lives in
 * the GPU's memory. Reading data back from a texture is not possible, so we
 * simply maintain a framebuffer in RAM since Ion::Display::pullRect expects to
 * be able to read pixel data back.
 * A side effect is that we rewrite the whole texture when redrawing the screen.
 * This might not be the most efficient way since sending pixels to the GPU is
 * rather expensive.
 * This is also very useful when running headless because we can easily log the
 * framebuffer to a PNG file. */

static KDColor sPixels[Ion::Display::Width * Ion::Display::Height];
static bool sFrameBufferActive = false;

namespace Ion {
namespace Display {

static KDFrameBuffer sFrameBuffer =
    KDFrameBuffer(sPixels, KDSize(Width, Height));

void pushRect(KDRect r, const KDColor* pixels) {
  if (sFrameBufferActive) {
    Simulator::Window::setNeedsRefresh();
    sFrameBuffer.pushRect(r, pixels);
  }
}

void pushRectUniform(KDRect r, KDColor c) {
  if (sFrameBufferActive) {
    Simulator::Window::setNeedsRefresh();
    sFrameBuffer.pushRectUniform(r, c);
  }
}

void pullRect(KDRect r, KDColor* pixels) {
  if (sFrameBufferActive) {
    sFrameBuffer.pullRect(r, pixels);
  }
}

}  // namespace Display
}  // namespace Ion

namespace Ion {
namespace Simulator {
namespace Framebuffer {

const KDColor* address() { return sPixels; }

void setActive(bool enabled) { sFrameBufferActive = enabled; }

}  // namespace Framebuffer
}  // namespace Simulator
}  // namespace Ion
