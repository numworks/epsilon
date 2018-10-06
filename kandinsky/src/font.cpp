#include "font.h"
#include <assert.h>

void decompress(const uint8_t * source, int sourceLength, uint8_t * output, int outputLength);

void KDFont::fetchGreyscaleGlyphForChar(char c, uint8_t * greyscaleBuffer) const {
  decompress(
    compressedGlyphData(c),
    compressedGlyphDataSize(c),
    greyscaleBuffer,
    m_glyphWidth*m_glyphHeight
  );
}

void KDFont::fetchGlyphForChar(char c, const KDFont::RenderPalette & renderPalette, KDColor * pixelBuffer) const {
  /* Since a greyscale value is smaller than a color value (see assertion), we
   * can store the temporary greyscale values in the output pixel buffer.
   * What's great is that now, if we fill the pixel buffer right-to-left with
   * colors derived from the temporary greyscale values, we will never overwrite
   * the remaining grayscale values since those are smaller. So we can avoid a
   * separate buffer for the temporary greyscale values. */
  assert(k_bitsPerPixel < 8*sizeof(KDColor));
  uint8_t * greyscaleBuffer = reinterpret_cast<uint8_t *>(pixelBuffer);
  fetchGreyscaleGlyphForChar(c, greyscaleBuffer);

  uint8_t mask = (0xFF >> (8-k_bitsPerPixel));
  int pixelIndex = m_glyphWidth * m_glyphHeight;
  int greyscaleByteIndex = pixelIndex / k_bitsPerPixel;
  while (pixelIndex >= 0) {
    uint8_t greyscaleByte = greyscaleBuffer[greyscaleByteIndex--];
    assert(greyscaleByteIndex >= 0);
    for (int j=0; j<8/k_bitsPerPixel; j++) {
      uint8_t greyscale = greyscaleByte & mask;
      greyscaleByte = greyscaleByte >> k_bitsPerPixel;
      pixelBuffer[pixelIndex--] = renderPalette.colorAtIndex(greyscale);
    }
  }
}

constexpr int glyphPixelCount = 12;

void drawString(const char * text, KDColor textColor, KDColor backgroundColor) {
  const KDFont * font = KDFont::LargeFont;
  KDFont::RenderPalette palette = font->renderPalette(textColor, backgroundColor);

  assert(glyphPixelCount >= font->glyphWidth() * font->glyphHeight());

  char c;
  do {
    char c = text[0];
    KDColor glyph[glyphPixelCount];
    font->fetchGlyphForChar(c, palette, glyph);
  } while (c != 0);
}
