#include <assert.h>
#include <ion/display.h>
#include <ion/unicode/utf8_decoder.h>
#include <kandinsky/context.h>
#include <kandinsky/font.h>

#include <cmath>

constexpr static int k_tabCharacterWidth = 4;

KDPoint KDContext::alignAndDrawSingleLineString(const char* text, KDPoint p,
                                                KDSize frame,
                                                float horizontalAlignment,
                                                KDGlyph::Style style,
                                                int maxLength) {
  KDSize textSize = KDFont::Font(style.font)->stringSize(text, maxLength);
  assert(textSize.width() <= frame.width() &&
         textSize.height() <= frame.height());
  KDPoint origin(p.x() + std::round(horizontalAlignment *
                                    (frame.width() - textSize.width())),
                 p.y());
  return drawString(text, origin, style, maxLength);
}

KDPoint KDContext::alignAndDrawString(const char* text, KDPoint p, KDSize frame,
                                      KDGlyph::Format format, int maxLength) {
  assert(format.horizontalAlignment >= 0.0f &&
         format.horizontalAlignment <= 1.0f &&
         format.verticalAlignment >= 0.0f && format.verticalAlignment <= 1.0f);
  /* Align vertically
   * Then split lines and horizontal-align each independently */
  KDSize textSize =
      KDFont::Font(format.style.font)->stringSize(text, maxLength);
  assert(textSize.width() <= frame.width() &&
         textSize.height() <= frame.height());
  // We ceil vertical alignment to prefer shifting down than up.
  KDPoint origin(p.x(),
                 p.y() + std::ceil(format.verticalAlignment *
                                   (frame.height() - textSize.height())));
  KDSize lineFrame =
      KDSize(frame.width(), KDFont::GlyphHeight(format.style.font));

  UTF8Decoder decoder(text);
  const char* startLine = text;
  CodePoint codePoint = decoder.nextCodePoint();
  const char* codePointPointer = decoder.stringPosition();
  while (codePoint != UCodePointNull &&
         (maxLength < 0 || codePointPointer < text + maxLength)) {
    if (codePoint == UCodePointLineFeed) {
      alignAndDrawSingleLineString(startLine, origin, lineFrame,
                                   format.horizontalAlignment, format.style,
                                   codePointPointer - startLine - 1);
      startLine = codePointPointer;
      origin = KDPoint(origin.x(), origin.y() + lineFrame.height());
    }
    codePoint = decoder.nextCodePoint();
    codePointPointer = decoder.stringPosition();
  }
  // Last line
  return alignAndDrawSingleLineString(startLine, origin, frame,
                                      format.horizontalAlignment, format.style,
                                      maxLength + text - startLine);
}

KDPoint KDContext::drawString(const char* text, KDPoint p, KDGlyph::Style style,
                              int maxByteLength) {
  KDPoint position = p;
  KDSize glyphSize = KDFont::GlyphSize(style.font);
  KDFont::RenderPalette palette =
      KDFont::Font(style.font)
          ->renderPalette(style.glyphColor, style.backgroundColor);
  KDFont::GlyphBuffer glyphBuffer;

  UTF8Decoder decoder(text);
  const char* codePointPointer = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull &&
         (maxByteLength < 0 || codePointPointer < text + maxByteLength)) {
    codePointPointer = decoder.stringPosition();
    if (codePoint == UCodePointLineFeed) {
      assert(position.y() < KDCOORDINATE_MAX - glyphSize.height());
      position = KDPoint(0, position.y() + glyphSize.height());
      codePoint = decoder.nextCodePoint();
    } else if (codePoint == UCodePointCarriageReturn) {
      // Ignore '\r' that are added for compatibility
      codePoint = decoder.nextCodePoint();
    } else if (codePoint == UCodePointTabulation) {
      position = position.translatedBy(
          KDPoint(k_tabCharacterWidth * glyphSize.width(), 0));
      codePoint = decoder.nextCodePoint();
    } else {
      assert(!codePoint.isCombining());
      // We don't want to draw '�'
      assert(KDFont::Font(style.font)->indexForCodePoint(codePoint) !=
             KDFont::k_indexForReplacementCharacterCodePoint);
      KDFont::Font(style.font)
          ->setGlyphGrayscalesForCodePoint(codePoint, &glyphBuffer);
      codePoint = decoder.nextCodePoint();
      while (codePoint.isCombining()) {
        KDFont::Font(style.font)
            ->accumulateGlyphGrayscalesForCodePoint(codePoint, &glyphBuffer);
        codePointPointer = decoder.stringPosition();
        codePoint = decoder.nextCodePoint();
      }
      KDFont::Font(style.font)->colorizeGlyphBuffer(&palette, &glyphBuffer);
      // Push the character on the screen
      fillRectWithPixels(
          KDRect(position, glyphSize), glyphBuffer.colorBuffer(),
          glyphBuffer.colorBuffer()  // It's OK to trash the content of the
                                     // color buffer since we'll re-fetch it for
                                     // the next char anyway
      );
      position = position.translatedBy(KDPoint(glyphSize.width(), 0));
    }
  }
  return position;
}
