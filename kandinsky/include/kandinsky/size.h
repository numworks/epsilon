#ifndef KANDINSKY_SIZE_H
#define KANDINSKY_SIZE_H

#include <kandinsky/coordinate.h>

class KDSize {
public:
  constexpr KDSize(KDCoordinate width, KDCoordinate height) :
    m_width(width), m_height(height) {}
  constexpr KDCoordinate width() const { return m_width; }
  constexpr KDCoordinate height() const { return m_height; }
private:
  KDCoordinate m_width;
  KDCoordinate m_height;
};

constexpr KDSize KDSizeZero = KDSize(0,0);

#endif
