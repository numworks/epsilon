#ifndef ESCHER_IMAGE_H
#define ESCHER_IMAGE_H

#include <kandinsky.h>

class Image {
public:
  constexpr Image(KDCoordinate width, KDCoordinate height, const KDColor * pixels) :
    m_width(width), m_height(height), m_pixels(pixels) {}
  KDCoordinate width() const { return m_width; }
  KDCoordinate height() const { return m_height; }
  const KDColor * pixels() const { return m_pixels; }
private:
  KDCoordinate m_width;
  KDCoordinate m_height;
  const KDColor * m_pixels;
};

#endif
