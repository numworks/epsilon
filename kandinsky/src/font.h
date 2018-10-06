#ifndef KANDINSKY_FONT_H
#define KANDINSKY_FONT_H

#include <stdint.h>
#include <kandinsky/coordinate.h>
#include "palette.h"

class KDFont {
private:
  static constexpr int k_bitsPerPixel = 4;
public:
  static const KDFont * LargeFont;
  static const KDFont * SmallFont;

  using RenderPalette = KDPalette<k_bitsPerPixel>;
  void fetchGlyphForChar(char c, const RenderPalette & renderPalette, KDColor * pixelBuffer) const;
  RenderPalette renderPalette(KDColor textColor, KDColor backgroundColor) const {
    return RenderPalette::Gradient(textColor, backgroundColor);
  }
  KDCoordinate glyphWidth() const { return m_glyphWidth; }
  KDCoordinate glyphHeight() const { return m_glyphWidth; }

  constexpr KDFont(KDCoordinate glyphWidth, KDCoordinate glyphHeight, const uint16_t * glyphDataOffset, const uint8_t * data) :
    m_glyphWidth(glyphWidth), m_glyphHeight(glyphHeight), m_glyphDataOffset(glyphDataOffset), m_data(data) { }
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
    return static_cast<uint8_t>(c);
  }

  KDCoordinate m_glyphWidth;
  KDCoordinate m_glyphHeight;
  const uint16_t * m_glyphDataOffset;
  const uint8_t * m_data;
};

#endif
