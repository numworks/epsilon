#include <assert.h>
#include <kandinsky/font.h>
#include "external/lz4/lz4.h"

constexpr static int k_tabCharacterWidth = 4;

KDSize KDFont::stringSize(const char * text) const {
  if (text == nullptr) {
    return KDSizeZero;
  }
  KDSize stringSize = KDSize(0, m_glyphSize.height());
  while (*text != 0) {
    KDSize cSize = KDSize(m_glyphSize.width(), 0);
    if (*text == '\t') {
      cSize = KDSize(k_tabCharacterWidth*m_glyphSize.width(), 0);
    }
    if (*text == '\n') {
      cSize = KDSize(0, m_glyphSize.height());
    }
    stringSize = KDSize(stringSize.width()+cSize.width(), stringSize.height()+cSize.height());
    text++;
  }
  return stringSize;
}

void KDFont::fetchGreyscaleGlyphForChar(char c, uint8_t * greyscaleBuffer) const {
  //TODO: If debug, use LZ4_decompress_safe, otherwise LZ4_decompress_fast
  int resultSize = LZ4_decompress_safe(
    reinterpret_cast<const char *>(compressedGlyphData(c)),
    reinterpret_cast<char *>(greyscaleBuffer),
    compressedGlyphDataSize(c),
    m_glyphSize.width() * m_glyphSize.height() * k_bitsPerPixel/8
  );
  (void)resultSize; // Silence the "variable unused" warning if assertions are not enabled
  assert(resultSize == m_glyphSize.width() * m_glyphSize.height() * k_bitsPerPixel/8);
}

void KDFont::fetchGlyphForChar(char c, const KDFont::RenderPalette * renderPalette, KDColor * pixelBuffer) const {
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
  int pixelIndex = m_glyphSize.width() * m_glyphSize.height() - 1; // Let's start at the final pixel
  int greyscaleByteIndex = pixelIndex * k_bitsPerPixel / 8;
  while (pixelIndex >= 0) {
    assert(greyscaleByteIndex == pixelIndex * k_bitsPerPixel / 8);
    uint8_t greyscaleByte = greyscaleBuffer[greyscaleByteIndex--]; // We consume a greyscale byte...
    for (int j=0; j<8/k_bitsPerPixel; j++) { // .. and we'll output 8/k_bits pixels
      uint8_t greyscale = greyscaleByte & mask;
      greyscaleByte = greyscaleByte >> k_bitsPerPixel;
      assert(pixelIndex >= 0);
      pixelBuffer[pixelIndex--] = renderPalette->colorAtIndex(greyscale);
    }
  }
}
