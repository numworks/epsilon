#include <kandinsky/font.h>
extern "C" {
#include <kandinsky/fonts/code_points.h>
}
#include <assert.h>
#include <ion.h>
#include <ion/unicode/utf8_decoder.h>

#include <algorithm>

constexpr static int k_tabCharacterWidth = 4;

KDSize KDFont::stringSizeUntil(const char* text, const char* limit) const {
  if (text == nullptr || (limit != nullptr && text >= limit)) {
    return KDSizeZero;
  }
  UTF8Decoder decoder(text);
  const char* currentStringPosition = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  KDCoordinate stringHeight = m_glyphSize.height();
  KDCoordinate stringWidth = 0;
  KDCoordinate lineStringWidth = 0;
  while (codePoint != UCodePointNull &&
         (limit == nullptr || currentStringPosition < limit)) {
    if (codePoint == UCodePointLineFeed) {
      stringWidth = std::max<KDCoordinate>(stringWidth, lineStringWidth);
      lineStringWidth = 0;
      stringHeight += m_glyphSize.height();
    } else if (codePoint == UCodePointTabulation) {
      lineStringWidth += k_tabCharacterWidth * m_glyphSize.width();
    } else if (!codePoint.isCombining()) {
      lineStringWidth += m_glyphSize.width();
    }
    currentStringPosition = decoder.stringPosition();
    codePoint = decoder.nextCodePoint();
  }
  stringWidth = std::max<KDCoordinate>(stringWidth, lineStringWidth);
  assert(stringWidth >= 0 && stringHeight >= 0);
  return KDSize(stringWidth, stringHeight);
}

void KDFont::setGlyphGrayscalesForCodePoint(CodePoint codePoint,
                                            GlyphBuffer* glyphBuffer) const {
  fetchGrayscaleGlyphAtIndex(indexForCodePoint(codePoint),
                             glyphBuffer->grayscaleBuffer());
}

void KDFont::setGlyphGrayscalesForCharacter(const char c,
                                            GlyphBuffer* glyphBuffer) const {
  fetchGrayscaleGlyphAtIndex(signedCharAsIndex(c),
                             glyphBuffer->grayscaleBuffer());
}

void KDFont::accumulateGlyphGrayscalesForCodePoint(
    CodePoint codePoint, GlyphBuffer* glyphBuffer) const {
  uint8_t* grayscaleBuffer = glyphBuffer->grayscaleBuffer();
  uint8_t* accumulationGrayscaleBuffer =
      glyphBuffer->secondaryGrayscaleBuffer();
  fetchGrayscaleGlyphAtIndex(indexForCodePoint(codePoint),
                             accumulationGrayscaleBuffer);
  for (int i = 0; i < m_glyphSize.width() * m_glyphSize.height(); i++) {
    grayscaleBuffer[i] |= accumulationGrayscaleBuffer[i];
  }
}

void KDFont::fetchGrayscaleGlyphAtIndex(KDFont::GlyphIndex index,
                                        uint8_t* grayscaleBuffer) const {
  Ion::decompress(
      compressedGlyphData(index), grayscaleBuffer,
      compressedGlyphDataSize(index),
      m_glyphSize.width() * m_glyphSize.height() * k_grayscaleBitsPerPixel / 8);
}

void KDFont::colorizeGlyphBuffer(const RenderPalette* renderPalette,
                                 GlyphBuffer* glyphBuffer) const {
  /* Since a grayscale value is smaller than a color value (see assertion), we
   * can store the temporary grayscale values in the output pixel buffer.
   * What's great is that now, if we fill the pixel buffer right-to-left with
   * colors derived from the temporary grayscale values, we will never overwrite
   * the remaining grayscale values since those are smaller. So we can avoid a
   * separate buffer for the temporary grayscale values. */
  assert(k_grayscaleBitsPerPixel < 8 * sizeof(KDColor));

  uint8_t* grayscaleBuffer = glyphBuffer->grayscaleBuffer();
  KDColor* colorBuffer = glyphBuffer->colorBuffer();

  uint8_t mask = (0xFF >> (8 - k_grayscaleBitsPerPixel));
  // Let's start at the final pixel
  int pixelIndex = m_glyphSize.width() * m_glyphSize.height() - 1;
  int grayscaleByteIndex = pixelIndex * k_grayscaleBitsPerPixel / 8;
  while (pixelIndex >= 0) {
    assert(grayscaleByteIndex == pixelIndex * k_grayscaleBitsPerPixel / 8);
    // We consume a grayscale byte...
    uint8_t grayscaleByte = grayscaleBuffer[grayscaleByteIndex--];
    for (int j = 0; j < 8 / k_grayscaleBitsPerPixel;
         j++) {  // .. and we'll output 8/k_bits pixels
      uint8_t grayscale = grayscaleByte & mask;
      grayscaleByte = grayscaleByte >> k_grayscaleBitsPerPixel;
      assert(pixelIndex >= 0);
      colorBuffer[pixelIndex--] = renderPalette->colorAtIndex(grayscale);
    }
  }
}

KDFont::GlyphIndex KDFont::indexForCodePoint(CodePoint c) const {
  const CodePointIndexPair* currentPair = s_CodePointToGlyphIndex;
  const CodePointIndexPair* endPair =
      &s_CodePointToGlyphIndex[s_codePointPairsTableLength - 1];
  if (c < currentPair->codePoint()) {
    goto NoMatchingGlyph;
  }
  while (currentPair < endPair) {
    const CodePointIndexPair* nextPair = currentPair + 1;
    if (c < nextPair->codePoint()) {
      CodePoint lastCodePointOfCurrentPair =
          currentPair->codePoint() +
          (nextPair->glyphIndex() - currentPair->glyphIndex() - 1);
      if (c > lastCodePointOfCurrentPair) {
        goto NoMatchingGlyph;
      }
      return currentPair->glyphIndex() + (c - currentPair->codePoint());
    }
    currentPair = nextPair;
  }
  if (endPair->codePoint() == c) {
    return endPair->glyphIndex();
  }
NoMatchingGlyph:
  assert(CodePoints[k_indexForReplacementCharacterCodePoint] == 0xFFFD);
  return k_indexForReplacementCharacterCodePoint;
}

bool KDFont::CanBeWrittenWithGlyphs(const char* text) {
  UTF8Decoder decoder(text);
  CodePoint cp = decoder.nextCodePoint();
  while (cp != UCodePointNull) {
    if (privateLargeFont.indexForCodePoint(cp) ==
            k_indexForReplacementCharacterCodePoint ||
        privateSmallFont.indexForCodePoint(cp) ==
            k_indexForReplacementCharacterCodePoint) {
      return false;
    }
    cp = decoder.nextCodePoint();
  }
  return true;
}
