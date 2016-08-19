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

  KDCoordinate x() const { return m_x; }
  KDCoordinate y() const { return m_y; }
  KDPoint origin() const { return KDPoint(m_x, m_y); }
  KDCoordinate width() const { return m_width; }
  KDCoordinate height() const { return m_height; }
  KDSize size() const { return KDSize(m_width, m_height); }
  KDCoordinate top() const { return m_y; }
  KDCoordinate right() const { return m_x+m_width; }
  KDCoordinate bottom() const { return m_y+m_height; }
  KDCoordinate left() const { return m_x; }

  void setOrigin(KDPoint origin);
  void setSize(KDSize size);

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
