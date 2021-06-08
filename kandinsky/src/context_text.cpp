#include <assert.h>
#include <kandinsky/context.h>
#include <kandinsky/font.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/display.h>
#include <cmath>

constexpr static int k_tabCharacterWidth = 4;

KDPoint KDContext::drawString(const char * text, KDPoint p, const KDFont * font, KDColor textColor, KDColor backgroundColor, int maxByteLength) {
  return pushOrPullString(text, p, font, textColor, backgroundColor, maxByteLength, true);
}

KDPoint KDContext::alignAndDrawSingleLineString(const char * text, KDPoint p, KDSize frame, float horizontalAlignment, const KDFont * font, KDColor textColor, KDColor backgroundColor, int maxLength) {
  KDSize textSize = font->stringSize(text, maxLength);
  assert(textSize.width() <= frame.width() && textSize.height() <= frame.height());
  KDPoint origin(p.x() + std::round(horizontalAlignment * (frame.width() - textSize.width())),
                 p.y());
  return drawString(text, origin, font, textColor, backgroundColor, maxLength);
}

KDPoint KDContext::alignAndDrawString(const char * text, KDPoint p, KDSize frame, float horizontalAlignment, float verticalAlignment, const KDFont * font, KDColor textColor, KDColor backgroundColor, int maxLength) {
  assert(horizontalAlignment >= 0.0f && horizontalAlignment <= 1.0f && verticalAlignment >= 0.0f && verticalAlignment <= 1.0f);
  /* Align vertically
   * Then split lines and horizontal-align each independently */
  KDSize textSize = font->stringSize(text, maxLength);
  assert(textSize.width() <= frame.width() && textSize.height() <= frame.height());
  // We ceil vertical alignment to prefer shifting down than up.
  KDPoint origin(p.x(), p.y() + std::ceil(verticalAlignment * (frame.height() - textSize.height())));
  KDSize lineFrame = KDSize(frame.width(), font->glyphSize().height());

  UTF8Decoder decoder(text);
  const char * startLine = text;
  CodePoint codePoint = decoder.nextCodePoint();
  const char * codePointPointer = decoder.stringPosition();
  while (codePoint != UCodePointNull && (maxLength < 0 || codePointPointer < text + maxLength)) {
    if (codePoint == UCodePointLineFeed) {
      alignAndDrawSingleLineString(startLine, origin, lineFrame, horizontalAlignment, font,
                                               textColor, backgroundColor,
                                               codePointPointer - startLine - 1);
      startLine = codePointPointer;
      origin = KDPoint(origin.x(), origin.y() + lineFrame.height());
    }
    codePoint = decoder.nextCodePoint();
    codePointPointer = decoder.stringPosition();
  }
  // Last line
  return alignAndDrawSingleLineString(startLine, origin, frame, horizontalAlignment, font,
                                      textColor, backgroundColor, maxLength + text - startLine);
}

int KDContext::checkDrawnString(const char * text, KDPoint p, const KDFont * font, KDColor textColor, KDColor backgroundColor, int maxLength) {
  int numberOfFailedPixels = 0;
  pushOrPullString(text, p, font, textColor, backgroundColor, maxLength, false, &numberOfFailedPixels);
  return numberOfFailedPixels;
}

KDPoint KDContext::pushOrPullString(const char * text, KDPoint p, const KDFont * font, KDColor textColor, KDColor backgroundColor, int maxByteLength, bool push, int * result) {
  KDPoint position = p;
  KDSize glyphSize = font->glyphSize();
  KDFont::RenderPalette palette = font->renderPalette(textColor, backgroundColor);
  KDFont::GlyphBuffer glyphBuffer;

  UTF8Decoder decoder(text);
  const char * codePointPointer = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull && (maxByteLength < 0 || codePointPointer < text + maxByteLength)) {
    codePointPointer = decoder.stringPosition();
    assert(codePoint != UCodePointLineFeed);
    if (codePoint == UCodePointTabulation) {
      position = position.translatedBy(KDPoint(k_tabCharacterWidth * glyphSize.width(), 0));
      codePoint = decoder.nextCodePoint();
    } else {
      assert(!codePoint.isCombining());
      font->setGlyphGrayscalesForCodePoint(codePoint, &glyphBuffer);
      codePoint = decoder.nextCodePoint();
      while (codePoint.isCombining()) {
        font->accumulateGlyphGrayscalesForCodePoint(codePoint, &glyphBuffer);
        codePointPointer = decoder.stringPosition();
        codePoint = decoder.nextCodePoint();
      }
      font->colorizeGlyphBuffer(&palette, &glyphBuffer);
      if (push) {
        // Push the character on the screen
        fillRectWithPixels(
            KDRect(position, glyphSize),
            glyphBuffer.colorBuffer(),
            glyphBuffer.colorBuffer() // It's OK to trash the content of the color buffer since we'll re-fetch it for the next char anyway
            );
      } else {
        // Pull and compare the character from the screen
        assert(result != nullptr);
        *result = 0;
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
            *result = (*result)+1;
            break;
          }
        }
      }
      position = position.translatedBy(KDPoint(glyphSize.width(), 0));
    }
  }

  return position;
}
