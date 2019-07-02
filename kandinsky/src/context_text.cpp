#include <assert.h>
#include <kandinsky/context.h>
#include <kandinsky/font.h>
#include <ion/unicode/utf8_decoder.h>

constexpr static int k_tabCharacterWidth = 4;

KDPoint KDContext::drawString(const char * text, KDPoint p, const KDFont * font, KDColor textColor, KDColor backgroundColor, int maxByteLength) {
  KDPoint position = p;
  KDSize glyphSize = font->glyphSize();
  KDFont::RenderPalette palette = font->renderPalette(textColor, backgroundColor);

  KDFont::GlyphBuffer glyphBuffer;

  UTF8Decoder decoder(text);
  const char * codePointPointer = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull && (maxByteLength < 0 || codePointPointer < text + maxByteLength)) {
    codePointPointer = decoder.stringPosition();
    if (codePoint == UCodePointLineFeed) {
      position = KDPoint(0, position.y() + glyphSize.height());
      codePoint = decoder.nextCodePoint();
    } else if (codePoint == UCodePointTabulation) {
      position = position.translatedBy(KDPoint(k_tabCharacterWidth * glyphSize.width(), 0));
      codePoint = decoder.nextCodePoint();
    } else {
      assert(!codePoint.isCombining());
      font->setGlyphGreyscalesForCodePoint(codePoint, &glyphBuffer);
      codePoint = decoder.nextCodePoint();
      while (codePoint.isCombining()) {
        font->accumulateGlyphGreyscalesForCodePoint(codePoint, &glyphBuffer);
        codePointPointer = decoder.stringPosition();
        codePoint = decoder.nextCodePoint();
      }
      font->colorizeGlyphBuffer(&palette, &glyphBuffer);
      // Flush accumulated content
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
