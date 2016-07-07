#ifndef KANDINSKY_RECT_H
#define KANDINSKY_RECT_H

#include <kandinsky/coordinate.h>
#include <kandinsky/point.h>
#include <kandinsky/size.h>

class KDRect {
public:
  constexpr KDRect(KDCoordinate x, KDCoordinate y,
      KDCoordinate width, KDCoordinate height) :
    m_x(x), m_y(y), m_width(width), m_height(height) {}

  KDRect(KDPoint p, KDSize s);
  KDRect(KDCoordinate x, KDCoordinate y, KDSize s);
  KDRect(KDPoint p, KDCoordinate width, KDCoordinate height);

  KDCoordinate x() const;
  KDCoordinate y() const;
  KDPoint origin() const;
  KDCoordinate width() const;
  KDCoordinate height() const;
  KDSize size() const;
  KDCoordinate left() const;
  KDCoordinate right() const;
  KDCoordinate top() const;
  KDCoordinate bottom() const;

  KDRect translatedBy(KDPoint p) const;
  KDRect movedTo(KDPoint p) const;
  bool intersects(const KDRect & other) const;
  KDRect intersectedWith(const KDRect & other) const;
  KDRect unionedWith(const KDRect & other) const; // Returns the smallest rectangle containing r1 and r2
  bool contains(KDPoint p) const;
  bool isEmpty() const;

private:
  KDCoordinate m_x, m_y, m_width, m_height;
};

constexpr KDRect KDRectZero = KDRect(0,0,0,0);

#endif
