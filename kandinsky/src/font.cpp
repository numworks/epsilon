#include <assert.h>
#include <kandinsky/font.h>
#include <ion.h>

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
  Ion::decompress(
    compressedGlyphData(c),
    greyscaleBuffer,
    compressedGlyphDataSize(c),
    m_glyphSize.width() * m_glyphSize.height() * k_bitsPerPixel/8
  );
}

void KDFont::fetchGlyphForChar(char c, const KDFont::RenderPalette * renderPalette, KDColor * pixelBuffer) const {
  int pixelCount = m_glyphSize.width() * m_glyphSize.height() - 1;
  int charIndex = signedCharAsIndex(c);
  if (charIndex < 0 || charIndex >= k_numberOfGlyphs) {
    // There is no data for this glyph
    for (int i = 0; i < pixelCount; i++) {
      pixelBuffer[i] = KDColorBlack;
    }
    return;
  }
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
  int pixelIndex = pixelCount; // Let's start at the final pixel
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
