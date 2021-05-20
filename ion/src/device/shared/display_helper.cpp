#include "display_helper.h"
#include <drivers/display.h>
#include <ion/display.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace DisplayHelper {

void pushOrPullString(const char * text, KDCoordinate * yOffset, const KDFont * font, bool push, bool * success) {
  KDSize glyphSize = font->glyphSize();
  KDFont::RenderPalette palette = font->renderPalette(KDColorBlack, KDColorWhite);
  KDFont::GlyphBuffer glyphBuffer;
  size_t len = strlen(text);
  KDPoint position((Ion::Display::Width - len*glyphSize.width())/2, *yOffset);

  while (*text) {
    font->setGlyphGrayscalesForCharacter(*text++, &glyphBuffer);
    font->colorizeGlyphBuffer(&palette, &glyphBuffer);
    if (push) {
      // Push the character on the screen
      Ion::Device::Display::pushRect(KDRect(position, glyphSize), glyphBuffer.colorBuffer());
    } else {
      // Pull and compare the character from the screen
      assert(success != nullptr && *success);
      KDFont::GlyphBuffer workingGlyphBuffer;
      KDColor * workingColorBuffer = workingGlyphBuffer.colorBuffer();
      KDColor * colorBuffer = glyphBuffer.colorBuffer();
      for (int i = 0; i < glyphSize.height() * glyphSize.width(); i++) {
        workingColorBuffer[i] = KDColorRed;
      }
      /* Caution: Unlike fillRectWithPixels, pullRect accesses outside (0, 0,
       * Ion::Display::Width, Ion::Display::Height) might give weird data. */
      Ion::Display::pullRect(KDRect(position, glyphSize), workingColorBuffer);
      for (int k = 0; k < glyphSize.height() * glyphSize.width(); k++) {
        if (colorBuffer[k] != workingColorBuffer[k]) {
          *success = false;
          break;
        }
      }
      if (!*success) {
        break;
      }
    }
    position = position.translatedBy(KDPoint(glyphSize.width(), 0));
  }
  *yOffset += glyphSize.height();
}

void drawString(const char * text, KDCoordinate * yOffset, const KDFont * font) {
  pushOrPullString(text, yOffset, font, true, nullptr);
}

bool checkDrawnString(const char * text, KDCoordinate * yOffset, const KDFont * font) {
  bool success = true;
  pushOrPullString(text, yOffset, font, false, &success);
  return success;
}

}
}
}
