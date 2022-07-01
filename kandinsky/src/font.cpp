#include <kandinsky/font.h>
extern "C" {
#include <kandinsky/fonts/code_points.h>
}
#include <ion.h>
#include <ion/unicode/utf8_decoder.h>
#include <assert.h>
#include <algorithm>

constexpr static int k_tabCharacterWidth = 4;

KDSize KDFont::stringSizeUntil(const char * text, const char * limit) const {
  if (text == nullptr || (limit != nullptr && text >= limit)) {
    return KDSizeZero;
  }
  KDSize stringSize = KDSize(0, 0);
  KDSize lineSize = KDSize(0, m_glyphSize.height());

  UTF8Decoder decoder(text);
  const char * currentStringPosition = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull && (limit == nullptr || currentStringPosition < limit)) {
    KDCoordinate codePointWidth = m_glyphSize.width();
    if (codePoint == UCodePointLineFeed) {
      KDCoordinate width = std::max(lineSize.width(), stringSize.width());
      stringSize = KDSize(width, stringSize.height() + m_glyphSize.height());
      lineSize = KDSize(0, m_glyphSize.height());
      codePointWidth = 0;
    } else if (codePoint == UCodePointTabulation) {
      codePointWidth = k_tabCharacterWidth * m_glyphSize.width();
    } else if (codePoint.isCombining()) {
      codePointWidth = 0;
    }
    lineSize = KDSize(lineSize.width() + codePointWidth, lineSize.height());
    currentStringPosition = decoder.stringPosition();
    codePoint = decoder.nextCodePoint();
  }
  KDCoordinate width = std::max(lineSize.width(), stringSize.width());
  stringSize = KDSize(width, stringSize.height() + m_glyphSize.height());
  assert(stringSize.width() >= 0 && stringSize.height() >= 0);
  return stringSize;
}

const KDFont * KDFont::toItalic() const {
  if (this == KDFont::LargeFont) {
    return KDFont::ItalicLargeFont;
  } else if (this == KDFont::SmallFont) {
    return KDFont::ItalicSmallFont;
  }

  return this;
}

void KDFont::setGlyphGrayscalesForCodePoint(CodePoint codePoint, GlyphBuffer * glyphBuffer) const {
  fetchGrayscaleGlyphAtIndex(indexForCodePoint(codePoint), glyphBuffer->grayscaleBuffer());
}

void KDFont::accumulateGlyphGrayscalesForCodePoint(CodePoint codePoint, GlyphBuffer * glyphBuffer) const {
  uint8_t * grayscaleBuffer = glyphBuffer->grayscaleBuffer();
  uint8_t * accumulationGrayscaleBuffer = glyphBuffer->secondaryGrayscaleBuffer();
  fetchGrayscaleGlyphAtIndex(indexForCodePoint(codePoint), accumulationGrayscaleBuffer);
  for (int i=0; i<m_glyphSize.width()*m_glyphSize.height(); i++) {
    grayscaleBuffer[i] |= accumulationGrayscaleBuffer[i];
  }
}

void KDFont::fetchGrayscaleGlyphAtIndex(KDFont::GlyphIndex index, uint8_t * grayscaleBuffer) const {
  Ion::decompress(
    compressedGlyphData(index),
    grayscaleBuffer,
    compressedGlyphDataSize(index),
    m_glyphSize.width() * m_glyphSize.height() * k_bitsPerPixel/8
  );
}

void KDFont::colorizeGlyphBuffer(const RenderPalette * renderPalette, GlyphBuffer * glyphBuffer) const {
  /* Since a grayscale value is smaller than a color value (see assertion), we
   * can store the temporary grayscale values in the output pixel buffer.
   * What's great is that now, if we fill the pixel buffer right-to-left with
   * colors derived from the temporary grayscale values, we will never overwrite
   * the remaining grayscale values since those are smaller. So we can avoid a
   * separate buffer for the temporary grayscale values. */
  assert(k_bitsPerPixel < 8*sizeof(KDColor));

  uint8_t * grayscaleBuffer = glyphBuffer->grayscaleBuffer();
  KDColor * colorBuffer = glyphBuffer->colorBuffer();

  uint8_t mask = (0xFF >> (8-k_bitsPerPixel));
  int pixelIndex = m_glyphSize.width() * m_glyphSize.height() - 1; // Let's start at the final pixel
  int grayscaleByteIndex = pixelIndex * k_bitsPerPixel / 8;
  while (pixelIndex >= 0) {
    assert(grayscaleByteIndex == pixelIndex * k_bitsPerPixel / 8);
    uint8_t grayscaleByte = grayscaleBuffer[grayscaleByteIndex--]; // We consume a grayscale byte...
    for (int j=0; j<8/k_bitsPerPixel; j++) { // .. and we'll output 8/k_bits pixels
      uint8_t grayscale = grayscaleByte & mask;
      grayscaleByte = grayscaleByte >> k_bitsPerPixel;
      assert(pixelIndex >= 0);
      colorBuffer[pixelIndex--] = renderPalette->colorAtIndex(grayscale);
    }
  }
}

KDFont::GlyphIndex KDFont::indexForCodePoint(CodePoint c) const {
#define USE_BINARY_SEARCH 0
#if USE_BINARY_SEARCH
  int lowerBound = 0;
  int upperBound = m_tableLength;
  while (true) {
    int currentIndex = (lowerBound+upperBound)/2;
    // printf("Considering %d in [%d,%d]\n", currentIndex, lowerBound, upperBound);
    const CodePointIndexPair * currentPair = m_table + currentIndex;
    const CodePointIndexPair * nextPair = (currentIndex + 1) < m_tableLength ? currentPair + 1 : nullptr;
    // printf("At this point, currentPair->codePoint() = %d and c = %d\n", currentPair->codePoint(), c);
    if (currentPair->codePoint() == c) {
      return currentPair->glyphIndex();
    } else if (currentPair->codePoint() > c) {
      // We need to look below
      if (upperBound == currentIndex) {
        // There's nothing below. Error out.
        return 0;
      }
      upperBound = currentIndex;
      continue;
    } else if (nextPair == nullptr) {
      return 0;
    } else if (nextPair->codePoint() == c) {
      return nextPair->glyphIndex();
    } else if (nextPair->codePoint() < c) {
      // We need to look above
      if (lowerBound == currentIndex) {
        // There's nothing above. Error out.
        return 0;
      }
      lowerBound = currentIndex;
      continue;
    } else {
      // At this point,
      // currentPair->codePoint < c && nextPair != nullptr && nextPair->codePoint > c
      // Yay, it's over!
      // There can be an empty space between the currentPair and the nextPair
      // e.g. currentPair(3,1) and nextPair(9, 4)
      // means value at codePoints 3, 4, 5, 6, 7, 8, 9
      //     are glyph identifiers 1, ?, ?, ?, ?, ?, 4
      //                 solved as 1, 2, 3, 0, 0, 0, 4

      // Let's hunt down the zeroes
      CodePoint lastCodePointOfCurrentPair = currentPair->codePoint() + (nextPair->glyphIndex() - currentPair->glyphIndex() - 1);
      if (c > lastCodePointOfCurrentPair) {
        return 0;
      }
      return currentPair->glyphIndex() + (c - currentPair->codePoint());
    }
  }
#else
  const CodePointIndexPair * currentPair = m_table;
  const CodePointIndexPair * endPair = m_table + m_tableLength - 1;
  if (c < currentPair->codePoint()) {
    goto NoMatchingGlyph;
  }
  while (currentPair < endPair) {
    const CodePointIndexPair * nextPair = currentPair + 1;
    if (c < nextPair->codePoint()) {
      CodePoint lastCodePointOfCurrentPair = currentPair->codePoint() + (nextPair->glyphIndex() - currentPair->glyphIndex() - 1);
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
  assert(ExtendedCodePoints[IndexForReplacementCharacterCodePoint] == 0xFFFD);
  return IndexForReplacementCharacterCodePoint;
#endif
}

bool KDFont::CanBeWrittenWithGlyphs(const char * text) {
  UTF8Decoder decoder(text);
  CodePoint cp = decoder.nextCodePoint();
  while(cp != UCodePointNull) {
    if (LargeFont->indexForCodePoint(cp) == KDFont::IndexForReplacementCharacterCodePoint
     || SmallFont->indexForCodePoint(cp) == KDFont::IndexForReplacementCharacterCodePoint)
    {
      return false;
    }
    cp = decoder.nextCodePoint();
  }
  return true;
}
