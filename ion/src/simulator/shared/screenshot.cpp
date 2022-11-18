#include "screenshot.h"

#include <ion/display.h>
#include <ion/unicode/utf8_decoder.h>
#include <kandinsky/font.h>

#include "framebuffer.h"
#include "platform.h"
#include "window.h"

#include <cstdio>

namespace Ion {
namespace Simulator {

#ifndef NDEBUG
// Constexpr for event name drawing
constexpr static KDFont::Size k_fontSize = KDFont::Size::Large;
constexpr static int k_glyphWidth = KDFont::GlyphWidth(k_fontSize);
constexpr static int k_glyphHeight = KDFont::GlyphHeight(k_fontSize);
constexpr static int k_margin = 6;
constexpr static KDColor k_backgroundColor = KDColorBlack;
constexpr static KDColor k_glyphColor = KDColorWhite;
#endif

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

void Screenshot::captureStep(Events::Event nextEvent) {
  if (m_eachStep) {
    capture(nextEvent);
  }
}

#ifndef NDEBUG
static void drawEventNameInBuffer(Events::Event e, KDColor * pixelsBuffer, int width, int height, int abscissaOfDraw, int ordinateOfDraw) {
  if (!e.name()) {
    return;
  }

  KDFont::RenderPalette palette = KDFont::Font(k_fontSize)->renderPalette(k_glyphColor, k_backgroundColor);
  KDFont::GlyphBuffer glyphBuffer;
  UTF8Decoder m_decoder(e.name());
  CodePoint c = m_decoder.nextCodePoint();
  int currentAbscissa = abscissaOfDraw;

  while (c != UCodePointNull && currentAbscissa < width - k_glyphWidth) {
    KDFont::Font(k_fontSize)->setGlyphGrayscalesForCodePoint(c, &glyphBuffer);
    KDFont::Font(k_fontSize)->colorizeGlyphBuffer(&palette, &glyphBuffer);
    for (int i = 0; i < k_glyphWidth * k_glyphHeight; i++) {
      pixelsBuffer[(ordinateOfDraw + i /  k_glyphWidth) * width + currentAbscissa + i % k_glyphWidth] = glyphBuffer.colorBuffer()[i];
    }
    currentAbscissa += k_glyphWidth;
    c = m_decoder.nextCodePoint();
  }
}
#endif

void Screenshot::capture(Events::Event nextEvent) {
  int height = Display::Height;
  int width = Display::Width;
#ifndef NDEBUG
  bool drawEvent = nextEvent != Events::None;
  if (drawEvent) {
    height += k_glyphHeight + 2 * k_margin;
  }
#endif

  KDColor pixelsBuffer[height * width];
  for (int i = 0; i < Display::Height * width; i++) {
    pixelsBuffer[i] = Simulator::Framebuffer::address()[i];
  }

#ifndef NDEBUG
  if (drawEvent) {
    for (int i = Display::Height * width; i < height * width; i++) {
      pixelsBuffer[i] = k_backgroundColor;
    }
    drawEventNameInBuffer(nextEvent, pixelsBuffer, width, height, k_margin, Display::Height + k_margin);
  }
#endif

  if (m_path != nullptr) {
    char path[1024];
    std::sprintf(path, "%s/img-%04d.png", m_path, m_stepNumber++);

    Simulator::Platform::saveImage(pixelsBuffer, width,
                                   height, m_eachStep ? path : m_path);
  } else if (!Simulator::Window::isHeadless()) {
    Simulator::Platform::copyImageToClipboard(pixelsBuffer, width, height);
  }
}

Screenshot * Screenshot::commandlineScreenshot() {
  static Screenshot s_commandlineScreenshot;
  return &s_commandlineScreenshot;
}

}  // namespace Simulator
}  // namespace Ion
