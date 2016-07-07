#ifndef KANDINSKY_SIZE_H
#define KANDINSKY_SIZE_H

#include <kandinsky/coordinate.h>

struct KDSize {
  constexpr KDSize(KDCoordinate width, KDCoordinate height) :
    m_width(width), m_height(height) {}
  KDCoordinate width() const;
  KDCoordinate height() const;
private:
  KDCoordinate m_width;
  KDCoordinate m_height;
};



#endif
