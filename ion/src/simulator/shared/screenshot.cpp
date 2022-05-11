#include "screenshot.h"

#include <ion/display.h>

#include "framebuffer.h"
#include "platform.h"

namespace Ion {
namespace Simulator {

Screenshot::Screenshot(const char * path) { init(path); }

void Screenshot::init(const char * path) {
  if (path != m_path) {
    // Hack flag to write to buffer without enabling the SDL
    Simulator::Framebuffer::setActive(true);
  }

  m_path = path;
}

void Screenshot::capture() {
  if (m_path != nullptr) {
    Simulator::Platform::saveImage(Simulator::Framebuffer::address(), Display::Width,
                                   Display::Height, m_path);
  } else {
    Simulator::Platform::copyImageToClipboard(Simulator::Framebuffer::address(), Display::Width, Display::Height);
  }
}

Screenshot * Screenshot::commandlineScreenshot() {
  static Screenshot s_commandlineScreenshot;
  return &s_commandlineScreenshot;
}

}  // namespace Simulator
}  // namespace Ion
