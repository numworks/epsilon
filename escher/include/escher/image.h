#ifndef ESCHER_IMAGE_H
#define ESCHER_IMAGE_H

#include <kandinsky.h>

class Image {
public:
  constexpr Image(KDCoordinate width, KDCoordinate height, const uint8_t * compressedPixelData, uint16_t compressedPixelDataSize) :
    m_width(width), m_height(height), m_compressedPixelData(compressedPixelData), m_compressedPixelDataSize(compressedPixelDataSize) {}
  KDCoordinate width() const { return m_width; }
  KDCoordinate height() const { return m_height; }
  const uint8_t * compressedPixelData() const { return m_compressedPixelData; }
  uint16_t compressedPixelDataSize() const { return m_compressedPixelDataSize; }
private:
  KDCoordinate m_width;
  KDCoordinate m_height;
  const uint8_t * m_compressedPixelData;
  uint16_t m_compressedPixelDataSize;
};

#endif
