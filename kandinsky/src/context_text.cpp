#include <assert.h>
#include <kandinsky/context.h>
#include <kandinsky/font.h>

constexpr int maxGlyphPixelCount = 180;
constexpr static int k_tabCharacterWidth = 4;

KDPoint KDContext::drawString(const char * text, KDPoint p, const KDFont * font, KDColor textColor, KDColor backgroundColor, int maxLength) {

  KDPoint position = p;
  KDSize glyphSize = font->glyphSize();
  KDFont::RenderPalette palette = font->renderPalette(textColor, backgroundColor);

  KDColor glyph[maxGlyphPixelCount];

  const char * end = text + maxLength;
  while(*text != 0 && text != end) {
    if (*text == '\n') {
      position = KDPoint(0, position.y() + glyphSize.height());
    } else if (*text == '\t') {
      position = position.translatedBy(KDPoint(k_tabCharacterWidth * glyphSize.width(), 0));
    } else {
      // Fetch and draw glyph for current char
      font->fetchGlyphForChar(*text, &palette, glyph);
      fillRectWithPixels(
        KDRect(position, glyphSize.width(), glyphSize.height()),
        glyph,
        glyph // It's OK to trash the content of glyph since we'll re-fetch it for the next char anyway
      );
      position = position.translatedBy(KDPoint(glyphSize.width(), 0));
    }
    text++;
  }
  return position;
}
