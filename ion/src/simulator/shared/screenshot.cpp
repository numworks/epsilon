#include "screenshot.h"

#include <ion/display.h>
#include <ion/unicode/utf8_decoder.h>
#include <kandinsky/font.h>

#include <cstdio>

#include "framebuffer.h"
#include "platform.h"
#include "window.h"

namespace Ion {
namespace Simulator {

// Constexpr for event name drawing
constexpr static KDFont::Size k_fontSize = KDFont::Size::Large;
constexpr static int k_glyphHeight = KDFont::GlyphHeight(k_fontSize);
constexpr static int k_margin = 6;
#if DEBUG
constexpr static int k_glyphWidth = KDFont::GlyphWidth(k_fontSize);
constexpr static KDColor k_backgroundColor = KDColorBlack;
constexpr static KDColor k_glyphColor = KDColorWhite;
#endif

Screenshot::Screenshot(const char* path) { init(path); }

void Screenshot::init(const char* path, bool eachStep) {
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

#if DEBUG
static void drawEventNameInBuffer(Events::Event e, KDColor* pixelsBuffer,
                                  int width, int height, int abscissaOfDraw,
                                  int ordinateOfDraw) {
  if (!e.name()) {
    return;
  }

  KDFont::RenderPalette palette =
      KDFont::Font(k_fontSize)->renderPalette(k_glyphColor, k_backgroundColor);
  KDFont::GlyphBuffer glyphBuffer;
  UTF8Decoder m_decoder(e.name());
  CodePoint c = m_decoder.nextCodePoint();
  int currentAbscissa = abscissaOfDraw;

  while (c != UCodePointNull && currentAbscissa < width - k_glyphWidth) {
    KDFont::Font(k_fontSize)->setGlyphGrayscalesForCodePoint(c, &glyphBuffer);
    KDFont::Font(k_fontSize)->colorizeGlyphBuffer(&palette, &glyphBuffer);
    for (int i = 0; i < k_glyphWidth * k_glyphHeight; i++) {
      pixelsBuffer[(ordinateOfDraw + i / k_glyphWidth) * width +
                   currentAbscissa + i % k_glyphWidth] =
          glyphBuffer.colorBuffer()[i];
    }
    currentAbscissa += k_glyphWidth;
    c = m_decoder.nextCodePoint();
  }
}
#endif

void Screenshot::capture(Events::Event nextEvent) {
  constexpr static int k_maxHeight =
      Display::Height + k_glyphHeight + 2 * k_margin;
  constexpr static int k_width = Display::Width;
  int height = Display::Height;

  KDColor pixelsBuffer[k_maxHeight * k_width];
  for (int i = 0; i < height * k_width; i++) {
    pixelsBuffer[i] = Simulator::Framebuffer::address()[i];
  }

#if DEBUG
  if (nextEvent != Events::None) {
    height = k_maxHeight;
    for (int i = Display::Height * k_width; i < height * k_width; i++) {
      pixelsBuffer[i] = k_backgroundColor;
    }
    drawEventNameInBuffer(nextEvent, pixelsBuffer, k_width, height, k_margin,
                          Display::Height + k_margin);
  }
#endif

  if (m_path != nullptr) {
    char path[1024];
    std::sprintf(path, "%s/img-%04d.png", m_path, m_stepNumber++);

    Simulator::Platform::saveImage(pixelsBuffer, k_width, height,
                                   m_eachStep ? path : m_path);
  } else if (!Simulator::Window::isHeadless()) {
    Simulator::Platform::copyImageToClipboard(pixelsBuffer, k_width, height);
  }
}

Screenshot* Screenshot::commandlineScreenshot() {
  static Screenshot s_commandlineScreenshot;
  return &s_commandlineScreenshot;
}

}  // namespace Simulator
}  // namespace Ion
