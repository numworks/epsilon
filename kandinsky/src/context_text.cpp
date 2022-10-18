#include <assert.h>
#include <kandinsky/context.h>
#include <kandinsky/font.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/display.h>
#include <cmath>

constexpr static int k_tabCharacterWidth = 4;

KDPoint KDContext::alignAndDrawSingleLineString(const char * text, KDPoint p, KDSize frame, float horizontalAlignment, KDFont::Size font, KDColor textColor, KDColor backgroundColor, int maxLength) {
  KDSize textSize = KDFont::Font(font)->stringSize(text, maxLength);
  assert(textSize.width() <= frame.width() && textSize.height() <= frame.height());
  KDPoint origin(p.x() + std::round(horizontalAlignment * (frame.width() - textSize.width())),
                 p.y());
  return drawString(text, origin, font, textColor, backgroundColor, maxLength);
}

KDPoint KDContext::alignAndDrawString(const char * text, KDPoint p, KDSize frame, float horizontalAlignment, float verticalAlignment, KDFont::Size font, KDColor textColor, KDColor backgroundColor, int maxLength) {
  assert(horizontalAlignment >= 0.0f && horizontalAlignment <= 1.0f && verticalAlignment >= 0.0f && verticalAlignment <= 1.0f);
  /* Align vertically
   * Then split lines and horizontal-align each independently */
  KDSize textSize = KDFont::Font(font)->stringSize(text, maxLength);
  assert(textSize.width() <= frame.width() && textSize.height() <= frame.height());
  // We ceil vertical alignment to prefer shifting down than up.
  KDPoint origin(p.x(), p.y() + std::ceil(verticalAlignment * (frame.height() - textSize.height())));
  KDSize lineFrame = KDSize(frame.width(), KDFont::GlyphHeight(font));

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

KDPoint KDContext::drawString(const char * text, KDPoint p, KDFont::Size font, KDColor textColor, KDColor backgroundColor, int maxByteLength) {
  KDPoint position = p;
  KDSize glyphSize = KDFont::GlyphSize(font);
  KDFont::RenderPalette palette = KDFont::Font(font)->renderPalette(textColor, backgroundColor);
  KDFont::GlyphBuffer glyphBuffer;

  UTF8Decoder decoder(text);
  const char * codePointPointer = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull && (maxByteLength < 0 || codePointPointer < text + maxByteLength)) {
    codePointPointer = decoder.stringPosition();
    if (codePoint == UCodePointLineFeed) {
      assert(position.y() < KDCOORDINATE_MAX - glyphSize.height());
      position = KDPoint(0, position.y() + glyphSize.height());
      codePoint = decoder.nextCodePoint();
    } else if (codePoint == UCodePointCarriageReturn) {
      // Ignore '\r' that are added for compatibility
      codePoint = decoder.nextCodePoint();
    } else if (codePoint == UCodePointTabulation) {
      position = position.translatedBy(KDPoint(k_tabCharacterWidth * glyphSize.width(), 0));
      codePoint = decoder.nextCodePoint();
    } else {
      assert(!codePoint.isCombining());
      // We don't want to draw '�'
      assert(KDFont::Font(font)->indexForCodePoint(codePoint) != KDFont::k_indexForReplacementCharacterCodePoint);
      KDFont::Font(font)->setGlyphGrayscalesForCodePoint(codePoint, &glyphBuffer);
      codePoint = decoder.nextCodePoint();
      while (codePoint.isCombining()) {
        KDFont::Font(font)->accumulateGlyphGrayscalesForCodePoint(codePoint, &glyphBuffer);
        codePointPointer = decoder.stringPosition();
        codePoint = decoder.nextCodePoint();
      }
      KDFont::Font(font)->colorizeGlyphBuffer(&palette, &glyphBuffer);
      // Push the character on the screen
      fillRectWithPixels(
          KDRect(position, glyphSize),
          glyphBuffer.colorBuffer(),
          glyphBuffer.colorBuffer() // It's OK to trash the content of the color buffer since we'll re-fetch it for the next char anyway
          );
      position = position.translatedBy(KDPoint(glyphSize.width(), 0));
    }
  }
  return position;
}
