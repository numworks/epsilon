#ifndef ESCHER_IMAGE_H
#define ESCHER_IMAGE_H

#include <kandinsky/coordinate.h>

namespace Escher {

class Image {
 public:
  constexpr Image(KDCoordinate width, KDCoordinate height,
                  const uint8_t* compressedPixelData,
                  uint16_t compressedPixelDataSize,
                  bool hasTransparency = false)
      : m_width(width),
        m_height(height),
        m_compressedPixelData(compressedPixelData),
        m_compressedPixelDataSize(compressedPixelDataSize),
        m_hasTransparency(hasTransparency) {}
  KDCoordinate width() const { return m_width; }
  KDCoordinate height() const { return m_height; }
  const uint8_t* compressedPixelData() const { return m_compressedPixelData; }
  uint16_t compressedPixelDataSize() const { return m_compressedPixelDataSize; }
  bool hasTransparency() const { return m_hasTransparency; }

 private:
  KDCoordinate m_width;
  KDCoordinate m_height;
  const uint8_t* m_compressedPixelData;
  uint16_t m_compressedPixelDataSize;
  bool m_hasTransparency;
};

}  // namespace Escher

#endif
