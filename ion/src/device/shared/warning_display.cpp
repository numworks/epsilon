#include "warning_display.h"
#include <drivers/display.h>
#include <ion/display.h>
#include <kandinsky/font.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace WarningDisplay {

void drawString(const char * message, KDCoordinate & yOffset, const KDFont * font) {
  KDSize glyphSize = font->glyphSize();
  KDFont::RenderPalette palette = font->renderPalette(KDColorBlack, KDColorWhite);
  KDFont::GlyphBuffer glyphBuffer;
  size_t len = strlen(message);
  KDPoint position((Ion::Display::Width - len*glyphSize.width())/2, yOffset);

  while (*message) {
    font->setGlyphGrayscalesForCharacter(*message++, &glyphBuffer);
    font->colorizeGlyphBuffer(&palette, &glyphBuffer);
    // Push the character on the screen
    Ion::Device::Display::pushRect(KDRect(position, glyphSize), glyphBuffer.colorBuffer());
    position = position.translatedBy(KDPoint(glyphSize.width(), 0));
  }
  yOffset += glyphSize.height();
}

void showMessage(const char * const * messages, int numberOfMessages) {
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Device::Display::pushRectUniform(screen, KDColorWhite);
  // TODO EMILIE: Would it be more optimized to directly use an image of the warning?
  const char * title = messages[0];
  KDCoordinate currentHeight = 60;
  drawString(title, currentHeight, KDFont::LargeFont);
  currentHeight += KDFont::LargeFont->glyphSize().height();
  for (int j = 1; j < numberOfMessages; j++) {
    const char * message = messages[j];
    drawString(message, currentHeight, KDFont::SmallFont);
  }
}

}
}
}
