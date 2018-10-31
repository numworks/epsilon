#include <assert.h>
#include <kandinsky/font.h>
#include <ion.h>
#include <kandinsky/unicode/utf8decoder.h>

constexpr static int k_tabCharacterWidth = 4;

KDSize KDFont::stringSize(const char * text) const {
  if (text == nullptr) {
    return KDSizeZero;
  }
  KDSize stringSize = KDSize(0, m_glyphSize.height());

  UTF8Decoder decoder(text);
  Codepoint codepoint = decoder.nextCodepoint();
  while (codepoint != Null) {
    KDSize cSize = KDSize(m_glyphSize.width(), 0);
    if (codepoint == LineFeed) {
      cSize = KDSize(0, m_glyphSize.height());
      codepoint = decoder.nextCodepoint();
    } else if (codepoint == Tabulation) {
      cSize = KDSize(k_tabCharacterWidth*m_glyphSize.width(), 0);
    } else if (codepoint.isCombining()) {
      cSize = KDSizeZero;
    }
    stringSize = KDSize(stringSize.width()+cSize.width(), stringSize.height()+cSize.height());
    codepoint = decoder.nextCodepoint();
  }
  return stringSize;
}

void KDFont::setGlyphGreyscalesForCodepoint(Codepoint codepoint, GlyphBuffer * glyphBuffer) const {
  fetchGreyscaleGlyphAtIndex(indexForCodepoint(codepoint), glyphBuffer->greyscaleBuffer());
}

void KDFont::accumulateGlyphGreyscalesForCodepoint(Codepoint codepoint, GlyphBuffer * glyphBuffer) const {
  uint8_t * greyscaleBuffer = glyphBuffer->greyscaleBuffer();
  uint8_t * accumulationGreyscaleBuffer = glyphBuffer->secondaryGreyscaleBuffer();
  fetchGreyscaleGlyphAtIndex(indexForCodepoint(codepoint), accumulationGreyscaleBuffer);
  for (int i=0; i<m_glyphSize.width()*m_glyphSize.height(); i++) {
    greyscaleBuffer[i] |= accumulationGreyscaleBuffer[i];
  }
}

void KDFont::fetchGreyscaleGlyphAtIndex(KDFont::GlyphIndex index, uint8_t * greyscaleBuffer) const {
  Ion::decompress(
    compressedGlyphData(index),
    greyscaleBuffer,
    compressedGlyphDataSize(index),
    m_glyphSize.width() * m_glyphSize.height() * k_bitsPerPixel/8
  );
}

void KDFont::colorizeGlyphBuffer(const RenderPalette * renderPalette, GlyphBuffer * glyphBuffer) const {
  /* Since a greyscale value is smaller than a color value (see assertion), we
   * can store the temporary greyscale values in the output pixel buffer.
   * What's great is that now, if we fill the pixel buffer right-to-left with
   * colors derived from the temporary greyscale values, we will never overwrite
   * the remaining grayscale values since those are smaller. So we can avoid a
   * separate buffer for the temporary greyscale values. */
  assert(k_bitsPerPixel < 8*sizeof(KDColor));

  uint8_t * greyscaleBuffer = glyphBuffer->greyscaleBuffer();
  KDColor * colorBuffer = glyphBuffer->colorBuffer();

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
      colorBuffer[pixelIndex--] = renderPalette->colorAtIndex(greyscale);
    }
  }
}

KDFont::GlyphIndex KDFont::indexForCodepoint(Codepoint c) const {
#define USE_BINARY_SEARCH 0
#if USE_BINARY_SEARCH
  int lowerBound = 0;
  int upperBound = m_tableLength;
  while (true) {
    int currentIndex = (lowerBound+upperBound)/2;
    // printf("Considering %d in [%d,%d]\n", currentIndex, lowerBound, upperBound);
    const CodepointIndexPair * currentPair = m_table + currentIndex;
    const CodepointIndexPair * nextPair = currentIndex + 1 < m_tableLength ? currentPair + 1 : nullptr;
    // printf("At this point, currentPair->codepoint() = %d and c = %d\n", currentPair->codepoint(), c);
    if (currentPair->codepoint() == c) {
      return currentPair->glyphIndex();
    } else if (currentPair->codepoint() > c) {
      // We need to look below
      if (upperBound == currentIndex) {
        // There's nothing below. Error out.
        return 0;
      }
      upperBound = currentIndex;
      continue;
    } else if (nextPair == nullptr) {
      return 0;
    } else if (nextPair->codepoint() == c) {
      return nextPair->glyphIndex();
    } else if (nextPair->codepoint() < c) {
      // We need to look above
      if (lowerBound == currentIndex) {
        // There's nothing above. Error out.
        return 0;
      }
      lowerBound = currentIndex;
      continue;
    } else {
      // At this point,
      // currentPair->codepoint < c && nextPair != nullptr && nextPair->codepoint > c
      // Yay, it's over!
      // There can be an empty space between the currentPair and the nextPair
      // e.g. currentPair(3,1) and nextPair(9, 4)
      // means value at codepoints 3, 4, 5, 6, 7, 8, 9
      //     are glyph identifiers 1, ?, ?, ?, ?, ?, 4
      //                 solved as 1, 2, 3, 0, 0, 0, 4

      // Let's hunt down the zeroes
      Codepoint lastCodepointOfCurrentPair = currentPair->codepoint() + (nextPair->glyphIndex() - currentPair->glyphIndex() - 1);
      if (c > lastCodepointOfCurrentPair) {
        return 0;
      }
      return currentPair->glyphIndex() + (c - currentPair->codepoint());
    }
  }
#else
  const CodepointIndexPair * currentPair = m_table;
  if (c < currentPair->codepoint()) {
    return 0;
  }
  const CodepointIndexPair * endPair = m_table + m_tableLength - 1;
  while (currentPair < endPair) {
    const CodepointIndexPair * nextPair = currentPair + 1;
    if (c < nextPair->codepoint()) {
      Codepoint lastCodepointOfCurrentPair = currentPair->codepoint() + (nextPair->glyphIndex() - currentPair->glyphIndex() - 1);
      if (c > lastCodepointOfCurrentPair) {
        return 0;
      }
      return currentPair->glyphIndex() + (c - currentPair->codepoint());
    }
    currentPair = nextPair;
  }
  if (endPair->codepoint() == c) {
    return endPair->glyphIndex();
  }
  return 0;
#endif
}
