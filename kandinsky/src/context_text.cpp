#include <assert.h>
#include <kandinsky/context.h>
#include <kandinsky/font.h>
#include <kandinsky/unicode/utf8decoder.h>

constexpr static int k_tabCharacterWidth = 4;

KDPoint KDContext::drawString(const char * text, KDPoint p, const KDFont * font, KDColor textColor, KDColor backgroundColor, int maxLength) {
  KDPoint position = p;
  KDSize glyphSize = font->glyphSize();
  KDFont::RenderPalette palette = font->renderPalette(textColor, backgroundColor);

  KDFont::GlyphBuffer glyphBuffer;

  UTF8Decoder decoder(text);
  Codepoint codepoint = decoder.nextCodepoint();
  while (codepoint != Null) {
    if (codepoint == LineFeed) {
      position = KDPoint(0, position.y() + glyphSize.height());
      codepoint = decoder.nextCodepoint();
    } else if (codepoint == Tabulation) {
      position = position.translatedBy(KDPoint(k_tabCharacterWidth * glyphSize.width(), 0));
      codepoint = decoder.nextCodepoint();
    } else {
      assert(!codepoint.isCombining());
      font->setGlyphGreyscalesForCodepoint(codepoint, &glyphBuffer);
      codepoint = decoder.nextCodepoint();
      while (codepoint.isCombining()) {
        font->accumulateGlyphGreyscalesForCodepoint(codepoint, &glyphBuffer);
        codepoint = decoder.nextCodepoint();
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
