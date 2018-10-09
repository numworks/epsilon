#include "font.h"
#include <assert.h>
#include "external/lz4/lz4.h"

void KDFont::fetchGreyscaleGlyphForChar(char c, uint8_t * greyscaleBuffer) const {
  //TODO: If debug, use LZ4_decompress_safe, otherwise LZ4_decompress_fast
  int resultSize = LZ4_decompress_safe(
    reinterpret_cast<const char *>(compressedGlyphData(c)),
    reinterpret_cast<char *>(greyscaleBuffer),
    compressedGlyphDataSize(c),
    m_glyphWidth * m_glyphHeight * k_bitsPerPixel/8
  );
  assert(resultSize == m_glyphWidth * m_glyphHeight * k_bitsPerPixel/8);
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
  int pixelIndex = m_glyphWidth * m_glyphHeight - 1; // Let's start at the final pixel
  int greyscaleByteIndex = pixelIndex * k_bitsPerPixel / 8;
  while (pixelIndex >= 0) {
    assert(greyscaleByteIndex == pixelIndex * k_bitsPerPixel / 8);
    uint8_t greyscaleByte = greyscaleBuffer[greyscaleByteIndex--]; // We consume a greyscale byte...
    for (int j=0; j<8/k_bitsPerPixel; j++) { // .. and we'll output 8/k_bits pixels
      uint8_t greyscale = greyscaleByte & mask;
      greyscaleByte = greyscaleByte >> k_bitsPerPixel;
      assert(pixelIndex >= 0);
      pixelBuffer[pixelIndex--] = renderPalette.colorAtIndex(greyscale);
    }
  }
}
/*

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
*/
