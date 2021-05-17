#include "compare_screenshot.h"

#include <ion/display.h>

#include "framebuffer.h"
#include "platform.h"

namespace Ion {
namespace CompareScreenshot {

static const char * s_path = nullptr;

void saveCompareScreenshotTo(const char * path) {
  Simulator::Framebuffer::setActive(true);  // Hack flag to write to buffer without enabling the SDL
  s_path = path;
}
bool isCompareScreenshotEnabled() { return s_path != nullptr; }
void saveCompareScreenshot() {
  if (s_path != nullptr) {
    Simulator::Platform::saveImage(Simulator::Framebuffer::address(), Display::Width, Display::Height, s_path);
  }
}
}  // namespace CompareScreenshot
}  // namespace Ion