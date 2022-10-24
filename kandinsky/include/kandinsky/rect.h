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
  KDCoordinate right() const { return m_x + m_width - 1; }
  KDCoordinate bottom() const { return m_y + m_height - 1; }
  KDCoordinate left() const { return m_x; }

  KDPoint topLeft() const { return KDPoint(left(), top()); }
  KDPoint topRight() const { return KDPoint(right(), top()); }
  KDPoint bottomLeft() const { return KDPoint(left(), bottom()); }
  KDPoint bottomRight() const { return KDPoint(right(), bottom()); }

  bool operator ==(const KDRect &other) const {
    return (m_x == other.m_x && m_y == other.m_y
        && m_width == other.m_width && m_height == other.m_height);
  }

  void setOrigin(KDPoint origin);
  void setSize(KDSize size);

  KDRect translatedBy(KDPoint p) const;
  KDRect movedTo(KDPoint p) const;
  bool intersects(const KDRect & other) const;
  KDRect intersectedWith(const KDRect & other) const;
  KDRect unionedWith(const KDRect & other) const; // Returns the smallest rectangle containing r1 and r2
  KDRect differencedWith(const KDRect & other) const; // Returns the smallest rectangle containing r1\r2
  bool contains(KDPoint p) const;
  bool containsRect(const KDRect & other) const;
  bool isAbove(KDPoint p) const;
  bool isUnder(KDPoint p) const;
  bool isEmpty() const;

private:
  KDCoordinate m_x, m_y, m_width, m_height;
};

constexpr KDRect KDRectZero = KDRect(0, 0, 0, 0);

#endif
