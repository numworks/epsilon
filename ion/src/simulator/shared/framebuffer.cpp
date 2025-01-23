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

static KDColor
    sPixels[Ion::Display::WidthWithBorder * Ion::Display::HeightWithBorder];
static bool sFrameBufferActive = false;

constexpr static KDPoint k_frameOrigin{Ion::Display::Border,
                                       Ion::Display::Border};

namespace Ion {
namespace Display {

class IntializedFrameBuffer : public KDFrameBuffer {
 public:
  IntializedFrameBuffer(KDColor* pixels, KDSize size)
      : KDFrameBuffer(pixels, size) {
    pushRectUniform(bounds(), k_fillColor);
  }
  static constexpr KDColor k_fillColor = ION_DISPLAY_BORDER_COLOR;
};

static IntializedFrameBuffer sFrameBuffer =
    IntializedFrameBuffer(sPixels, KDSize(WidthWithBorder, HeightWithBorder));

void pushRect(KDRect r, const KDColor* pixels) {
  if (sFrameBufferActive) {
    Simulator::Window::setNeedsRefresh();
    sFrameBuffer.pushRect(r.translatedBy(k_frameOrigin), pixels);
  }
}

void pushRectUniform(KDRect r, KDColor c) {
  if (sFrameBufferActive) {
    Simulator::Window::setNeedsRefresh();
    sFrameBuffer.pushRectUniform(r.translatedBy(k_frameOrigin), c);
  }
}

void pullRect(KDRect r, KDColor* pixels) {
  if (sFrameBufferActive) {
    sFrameBuffer.pullRect(r.translatedBy(k_frameOrigin), pixels);
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
