#ifndef KANDINSKY_FONT_H
#define KANDINSKY_FONT_H

#include <ion/unicode/code_point.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/fonts/SmallFont.h>
#include <kandinsky/fonts/LargeFont.h>
#include <kandinsky/fonts/code_points.h>
#include <kandinsky/fonts/font_constants.h>
#include <kandinsky/size.h>
#include <stddef.h>
#include <stdint.h>
#include <algorithm>

#include "palette.h"

constexpr static int indexOf(const uint32_t * array, const int arraySize, const uint32_t value) {
  return arraySize == 0 ? -1 : value == *array ? 0 : 1 + indexOf(array + 1, arraySize - 1, value);
}

/* We use UTF-8 encoding. This means that a character is encoded as a code point
 * that uses between 1 and 4 bytes. Code points can be "combining", in which
 * case their glyph should be superimposed to the glyph of the previous code
 * point in the string. This is for instance used to print accents: the string
 * for the glyph 'è' is composed of the code point for 'e' followed by the
 * combining code point for '`'.
 * ASCII characters have the same encoding in ASCII and in UTF-8.
 *
 * We have a table of the glyphs we can draw (uint32_t CodePoints[] in
 * kandinsky/fonts/code_points.h).
 * A font contains a m_data[]  table which is the concatenation of
 * compressed glyph bitmaps (one per codepoint in CodePoints[]). We use the s_codePointPairsTable[]
 * to find the glyph index for a given codepoint: it contains the CodePointIndexPairs of the first
 * code point of each series of consecutive code points in the CodePoints table. m_glyphDataOffset[]
 * is used to find the location of the buffer for a given glyph index. */

class KDFont {
private:
  static const KDFont privateLargeFont;
  static const KDFont privateSmallFont;

public:
  enum class Size : bool {
    Small, // width = 7 / height = 14
    Large // width = 10 / height = 18
  };
  constexpr static KDCoordinate GlyphWidth(Size size) { return size == Size::Small ? SmallFont::k_glyphWidth : LargeFont::k_glyphWidth; }
  constexpr static KDCoordinate GlyphHeight(Size size) { return size == Size::Small ? SmallFont::k_glyphHeight : LargeFont::k_glyphHeight; }
  constexpr static KDSize GlyphSize(Size size) { return KDSize(GlyphWidth(size), GlyphHeight(size)); }
  constexpr static int k_maxGlyphPixelCount = std::max({SmallFont::k_glyphWidth * SmallFont::k_glyphHeight, LargeFont::k_glyphWidth * LargeFont::k_glyphHeight});
  constexpr static const KDFont * Font(Size size) { return size == Size::Small ? &privateSmallFont : &privateLargeFont; }

  static bool CanBeWrittenWithGlyphs(const char * text);

  KDSize stringSize(const char * text, int textLength = -1) const {
    return stringSizeUntil(text, textLength < 0 ? nullptr : text + textLength);
  }
  KDSize stringSizeUntil(const char * text, const char * limit) const;

  union GlyphBuffer {
  public:
    GlyphBuffer() {}  // Don't initialize either buffer
    KDColor * colorBuffer() { return m_colors; }
    uint8_t * grayscaleBuffer() { return m_grayscales; }
    uint8_t * secondaryGrayscaleBuffer() { return m_grayscales + k_maxGlyphPixelCount; }

  private:
    uint8_t m_grayscales[2 * k_maxGlyphPixelCount];
    KDColor m_colors[k_maxGlyphPixelCount];
  };

  // A GlyphIndex is the index of a codepoint in CodePoints[]
  using GlyphIndex = uint8_t;
  class CodePointIndexPair {
  public:
    constexpr CodePointIndexPair(CodePoint c, GlyphIndex i) : m_codePoint(c), m_glyphIndex(i) {}
    CodePoint codePoint() const { return m_codePoint; }
    GlyphIndex glyphIndex() const { return m_glyphIndex; }

  private:
    CodePoint m_codePoint;
    GlyphIndex m_glyphIndex;
  };
  constexpr static int k_indexForReplacementCharacterCodePoint = indexOf(CodePoints,
                                                                         NumberOfCodePoints,
                                                                         UCodePointReplacement);
  GlyphIndex indexForCodePoint(CodePoint c) const;

  void setGlyphGrayscalesForCodePoint(CodePoint codePoint, GlyphBuffer * glyphBuffer) const;
  void setGlyphGrayscalesForCharacter(char c, GlyphBuffer * glyphBuffer) const;
  void accumulateGlyphGrayscalesForCodePoint(CodePoint codePoint, GlyphBuffer * glyphBuffer) const;

  using RenderPalette = KDPalette<(1 << k_grayscaleBitsPerPixel)>;
  void colorizeGlyphBuffer(const RenderPalette * renderPalette, GlyphBuffer * glyphBuffer) const;

  RenderPalette renderPalette(KDColor textColor, KDColor backgroundColor) const {
    return RenderPalette::Gradient(textColor, backgroundColor);
  }

  constexpr KDFont(KDCoordinate glyphWidth,
                   KDCoordinate glyphHeight,
                   const uint16_t * glyphDataOffset,
                   const uint8_t * data) :
        m_glyphSize(glyphWidth, glyphHeight), m_glyphDataOffset(glyphDataOffset), m_data(data) {}

private:
  void fetchGrayscaleGlyphAtIndex(GlyphIndex index, uint8_t * grayscaleBuffer) const;

  const uint8_t * compressedGlyphData(GlyphIndex index) const {
    return &m_data[m_glyphDataOffset[index]];
  }

  uint16_t compressedGlyphDataSize(GlyphIndex index) const {
    return m_glyphDataOffset[index + 1] - m_glyphDataOffset[index];
  }

  int signedCharAsIndex(const char c) const {
    /* TODO this method and setGlyphGrayscalesForCharacter should be removed
     * they were (re)introduced to avoid pulling the CodePoint machinery into the kernel,
     * but it can probably still be done. */
    constexpr static uint8_t k_magicCharOffsetValue = CodePoints[0];
    int cInt = c;
    if (cInt < 0) {
      /* A char casted as int takes its value between -127 and +128, but we want
       * a positive value. -127 is thus 129, -126 is 130, etc. */
      cInt = 128 + (cInt - (-127) + 1);
    }
    return cInt - k_magicCharOffsetValue;
  }

  KDSize m_glyphSize;
  const uint16_t * m_glyphDataOffset;
  const uint8_t * m_data;

  static const CodePointIndexPair * s_CodePointToGlyphIndex;
  static const size_t s_codePointPairsTableLength;
};

#endif
