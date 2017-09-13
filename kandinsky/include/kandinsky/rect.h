#ifndef KANDINSKY_RECT_H
#define KANDINSKY_RECT_H

#include <kandinsky/coordinate.h>
#include <kandinsky/point.h>
#include <kandinsky/size.h>

/*   +-+-+-+-+-+
 *   |x| | | | |
 *   +-+-+-+-+-+
 *   | | | | | |
 *   +-+-+-+-+-+
 *   | | | | |o|
 *   +-+-+-+-+-+
 *
 *  Kandinsky rectangles are rectangles made of pixels. We defined them by the
 *  coordinates of the top left pixel and the rectangle dimensions in pixels.
 *  The pixel top left (located by the 'x' on the example) is at the coordinates
 *  (left(), top()) = (x(), y()). The pixel bottom right (located by a 'o' on
 *  the example) is at the coordinates (right(), bottom()). Also, the dimension
 *  of the rectangle is then (right()-left()+1, bottom()-top()+1).
 *  The example dimensions are (5, 3).
 *
 * */

class KDRect {
public:
  constexpr KDRect(KDCoordinate x, KDCoordinate y, KDCoordinate width, KDCoordinate height) :
    m_x(x), m_y(y), m_width(width), m_height(height) {}

  constexpr KDRect(KDPoint p, KDSize s) :
    m_x(p.x()), m_y(p.y()), m_width(s.width()), m_height(s.height()) {}
  constexpr KDRect(KDCoordinate x, KDCoordinate y, KDSize s):
    m_x(x), m_y(y), m_width(s.width()), m_height(s.height()) {}
  constexpr KDRect(KDPoint p, KDCoordinate width, KDCoordinate height) :
    m_x(p.x()), m_y(p.y()), m_width(width), m_height(height) {}

  constexpr KDCoordinate x() const { return m_x; }
  constexpr KDCoordinate y() const { return m_y; }
  constexpr KDPoint origin() const { return KDPoint(m_x, m_y); }
  constexpr KDCoordinate width() const { return m_width; }
  constexpr KDCoordinate height() const { return m_height; }
  constexpr KDSize size() const { return KDSize(m_width, m_height); }
  constexpr KDCoordinate top() const { return m_y; }
  constexpr KDCoordinate right() const { return m_x+m_width-1; }
  constexpr KDCoordinate rightp1() const { return m_x+m_width; }
  constexpr KDCoordinate bottom() const { return m_y+m_height-1; }
  constexpr KDCoordinate bottomp1() const { return m_y+m_height; }
  constexpr KDCoordinate left() const { return m_x; }

  constexpr KDPoint topLeft() const { return KDPoint(left(), top()); }
  constexpr KDPoint topRight() const { return KDPoint(right(), top()); }
  constexpr KDPoint bottomLeft() const { return KDPoint(left(), bottom()); }
  constexpr KDPoint bottomRight() const { return KDPoint(right(), bottom()); }

  constexpr bool operator ==(const KDRect &other) const {
    return (m_x == other.m_x && m_y == other.m_y
        && m_width == other.m_width && m_height == other.m_height);
  }

  void setOrigin(KDPoint origin) { m_x = origin.x(); m_y = origin.y(); }
  void setSize(KDSize size) { m_width = size.width(); m_height = size.height(); }

  constexpr KDRect translatedBy(KDPoint p) const {
    return KDRect(x() + p.x(), y() + p.y(), width(), height());
  }
  constexpr KDRect movedTo(KDPoint p) const {
    return KDRect(p.x(), p.y(), width(), height());
  }
  bool intersects(const KDRect & other) const;
  KDRect intersectedWith(const KDRect & other) const;
  KDRect unionedWith(const KDRect & other) const; // Returns the smallest rectangle containing r1 and r2
  KDRect differencedWith(const KDRect & other) const; // Returns the smallest rectangle containing r1\r2
  bool contains(KDPoint p) const;
  constexpr bool isAbove(KDPoint p) const {
    return (p.y() >= y());
  }
  constexpr bool isUnder(KDPoint p) const{
    return (p.y() <= bottom());
  }
  constexpr bool isEmpty() const {
    return (width() == 0 || height() == 0);
  }

private:
  KDCoordinate m_x, m_y, m_width, m_height;
};

constexpr KDRect KDRectZero = KDRect(0,0,0,0);

#endif
