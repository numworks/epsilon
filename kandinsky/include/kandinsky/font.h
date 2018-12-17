#ifndef KANDINSKY_FONT_H
#define KANDINSKY_FONT_H

#include <stdint.h>
#include <kandinsky/size.h>
#include <kandinsky/coordinate.h>
#include "palette.h"

class KDFont {
private:
  static constexpr int k_bitsPerPixel = 4;
  static const KDFont privateLargeFont;
  static const KDFont privateSmallFont;
public:
  static constexpr const KDFont * LargeFont = &privateLargeFont;
  static constexpr const KDFont * SmallFont = &privateSmallFont;

  KDSize stringSize(const char * text) const;

  using RenderPalette = KDPalette<(1<<k_bitsPerPixel)>;
  void fetchGlyphForChar(char c, const RenderPalette * renderPalette, KDColor * pixelBuffer) const;
  RenderPalette renderPalette(KDColor textColor, KDColor backgroundColor) const {
    return RenderPalette::Gradient(textColor, backgroundColor);
  }
  KDSize glyphSize() const { return m_glyphSize; }

  constexpr KDFont(KDCoordinate glyphWidth, KDCoordinate glyphHeight, const uint16_t * glyphDataOffset, const uint8_t * data) :
    m_glyphSize(glyphWidth, glyphHeight), m_glyphDataOffset(glyphDataOffset), m_data(data) { }
private:
  void fetchGreyscaleGlyphForChar(char c, uint8_t * greyscaleBuffer) const;

  const uint8_t * compressedGlyphData(char c) const {
    return m_data + m_glyphDataOffset[charAsIndex(c)];
  }
  uint16_t compressedGlyphDataSize(char c) const {
    return m_glyphDataOffset[charAsIndex(c)+1] - m_glyphDataOffset[charAsIndex(c)];
  }
  uint8_t charAsIndex(char c) const {
    // FIXME: This is most likely false for chars greater than 127
    return static_cast<uint8_t>(c) - k_magicCharOffsetValue;
  }
  int signedCharAsIndex(char c) const {
    int cInt = c;
    if (cInt < 0) {
      /* A char casted as int takes its value between -127 and +128, but we want
       * a positive value. -127 is thus 129, -126 is 130, etc. */
      cInt=128+(cInt-(-127)+1);
    }
    return cInt - k_magicCharOffsetValue;
  }

  KDSize m_glyphSize;
  const uint16_t * m_glyphDataOffset;
  const uint8_t * m_data;
  static constexpr uint8_t k_magicCharOffsetValue = 0x20; // FIXME: Value from kandinsky/fonts/rasterizer.c (CHARACTER_RANGE_START). 0x20 because we do not want have a glyph for the first 20 ASCII characters
  static constexpr uint8_t k_numberOfGlyphs = 120; // FIXME: Value from kandinsky/fonts/rasterizer.c (GLYPH_COUNT)
};

#endif
