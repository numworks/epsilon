#include "screenshot.h"

#include <ion/display.h>

#include "framebuffer.h"
#include "platform.h"

namespace Ion {
namespace Simulator {

static bool wasInited = false;

void Screenshot::init(const char * path) {
  if (!wasInited) {
    // Hack flag to write to buffer without enabling the SDL
    Simulator::Framebuffer::setActive(true);
  }

  m_path = path;
}

void Screenshot::capture() {
  if (m_path != nullptr) {
    Simulator::Platform::saveImage(Simulator::Framebuffer::address(), Display::Width, Display::Height, m_path);
  }
}

Screenshot * Screenshot::commandlineScreenshot() {
  static Screenshot s_commandlineScreenshot;
  return &s_commandlineScreenshot;
}

}  // namespace Simulator
}  // namespace Ion
