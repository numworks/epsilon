#ifndef ESCHER_IMAGE_H
#define ESCHER_IMAGE_H

#include <kandinsky.h>

class Image {
public:
  constexpr Image(KDCoordinate width, KDCoordinate height, const unsigned char * data) :
    m_width(width), m_height(height), m_data(data) {}
  KDCoordinate width() const { return m_width; }
  KDCoordinate height() const { return m_height; }
  const KDColor * pixels() const { return (const KDColor *)m_data; }
private:
  KDCoordinate m_width;
  KDCoordinate m_height;
  const unsigned char * m_data;
};

#endif
