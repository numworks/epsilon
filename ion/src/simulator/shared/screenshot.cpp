#include "screenshot.h"

#include <ion/display.h>

#include "framebuffer.h"
#include "platform.h"
#include "window.h"

#include <cstdio>

namespace Ion {
namespace Simulator {

Screenshot::Screenshot(const char * path) { init(path); }

void Screenshot::init(const char * path, bool eachStep) {
  if (path != m_path) {
    // Hack flag to write to buffer without enabling the SDL
    Simulator::Framebuffer::setActive(true);
  }

  m_path = path;
  m_eachStep = eachStep;
  m_stepNumber = 0;
}

void Screenshot::captureStep() {
  if (m_eachStep) {
    capture();
  }
}

void Screenshot::capture() {
  if (m_path != nullptr) {
    char path[1024];
    std::sprintf(path, "%s/img-%04d.png", m_path, m_stepNumber++);
    Simulator::Platform::saveImage(Simulator::Framebuffer::address(), Display::Width,
                                   Display::Height, m_eachStep ? path : m_path);
  } else if (!Simulator::Window::isHeadless()) {
    Simulator::Platform::copyImageToClipboard(Simulator::Framebuffer::address(), Display::Width, Display::Height);
  }
}

Screenshot * Screenshot::commandlineScreenshot() {
  static Screenshot s_commandlineScreenshot;
  return &s_commandlineScreenshot;
}

}  // namespace Simulator
}  // namespace Ion
