#include "screenshot.h"

#include <ion.h>
#include <ion/display.h>
#include <ion/keyboard/layout_events.h>
#include <kandinsky/font.h>
#include <omg/print.h>
#include <omg/utf8_decoder.h>

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
#if DEBUG && ION_LOG_EVENTS_NAME
constexpr static int k_glyphWidth = KDFont::GlyphMaxWidth(k_fontSize);  // TODO
constexpr static KDColor k_backgroundColor = KDColorBlack;
constexpr static KDColor k_glyphColor = KDColorWhite;
#endif

Screenshot::Screenshot(const char* path) { init(path); }

void Screenshot::init(const char* path, bool eachStep, bool computeCRC32) {
  if (path != m_path || computeCRC32) {
    /* Hack flag to write to buffer without enabling the SDL.
     * When computing CRC32, we don't need a path but we need to activate frame
     * buffer. */
    Simulator::Framebuffer::setActive(true);
  }

  m_path = path;
  m_eachStep = eachStep;
  m_stepNumber = -1;
  m_computeCRC32 = computeCRC32;
}

#if DEBUG && ION_LOG_EVENTS_NAME
static void drawEventNameInBuffer(Events::Event e, KDColor* pixelsBuffer,
                                  int width, int height, int abscissaOfDraw,
                                  int ordinateOfDraw) {
  if (!e.name() || e == Events::None) {
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

static void printInConsole(uint32_t crc) {
  char crcBuffer[OMG::Print::MaxLengthOfUInt32(OMG::Base::Hexadecimal) + 1];
  size_t length = OMG::Print::UInt32(OMG::Base::Hexadecimal, crc,
                                     OMG::Print::LeadingZeros::Keep, crcBuffer,
                                     sizeof(crcBuffer));
  crcBuffer[length] = 0;
  Ion::Console::writeLine("CRC32 of all screenshots: ", false);
  Ion::Console::writeLine(crcBuffer);
}

void Screenshot::capture(Events::Event nextEvent) {
  m_stepNumber++;
  bool isLastScreenshot = nextEvent == Events::None;
  if (!isLastScreenshot && !m_eachStep) {
    return;
  }

  constexpr static int k_maxHeight =
      Display::HeightWithBorder + k_glyphHeight + 2 * k_margin;
  constexpr static int k_width = Display::WidthWithBorder;
  int height = Display::HeightWithBorder;

  KDColor pixelsBuffer[k_maxHeight * k_width];
  for (int i = 0; i < height * k_width; i++) {
    pixelsBuffer[i] = Simulator::Framebuffer::address()[i];
  }

  if (m_computeCRC32) {
    uint32_t newCRC32 = Ion::crc32Word(
        reinterpret_cast<const uint16_t*>(pixelsBuffer), height * k_width);
    if (m_stepNumber == 0) {
      m_CRC32 = newCRC32;
    } else {
      uint32_t crc32Results[2] = {m_CRC32, newCRC32};
      m_CRC32 = Ion::crc32DoubleWord(crc32Results, 2);
    }
    if (isLastScreenshot) {
      printInConsole(m_CRC32);
    }
    return;
  }

#if DEBUG && ION_LOG_EVENTS_NAME
  if (m_eachStep) {
    height = k_maxHeight;
    for (int i = Display::HeightWithBorder * k_width; i < height * k_width;
         i++) {
      pixelsBuffer[i] = k_backgroundColor;
    }
    drawEventNameInBuffer(nextEvent, pixelsBuffer, k_width, height, k_margin,
                          Display::HeightWithBorder + k_margin);
  }
#endif

  if (m_path != nullptr) {
    constexpr size_t pathSize = 1024;
    char path[pathSize];
    std::snprintf(path, pathSize, "%s/img-%04d.png", m_path, m_stepNumber);

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
