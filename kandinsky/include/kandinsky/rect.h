#ifndef KANDINSKY_RECT_H
#define KANDINSKY_RECT_H

#include <ion/display_constants.h>
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
    m_origin(x, y), 
    m_size(width, height) 
  {}
  KDRect(KDPoint p, KDSize s) :
    m_origin(p),
    m_size(s)
  {}
  KDRect(KDCoordinate x, KDCoordinate y, KDSize s) :
    m_origin(x, y),
    m_size(s)
  {}
  KDRect(KDPoint p, KDCoordinate width, KDCoordinate height) :
    m_origin(p),
    m_size(width, height)
  {}

  KDCoordinate x() const { return m_origin.x(); }
  KDCoordinate y() const { return m_origin.y(); }
  KDPoint origin() const { return m_origin; }
  KDCoordinate width() const { return m_size.width(); }
  KDCoordinate height() const { return m_size.height(); }
  KDSize size() const { return m_size; }
  KDCoordinate top() const { return y(); }
  KDCoordinate right() const { return x() + width() - 1; }
  KDCoordinate bottom() const { return y() + height() - 1; }
  KDCoordinate left() const { return x(); }

  KDPoint topLeft() const { return KDPoint(left(), top()); }
  KDPoint topRight() const { return KDPoint(right(), top()); }
  KDPoint bottomLeft() const { return KDPoint(left(), bottom()); }
  KDPoint bottomRight() const { return KDPoint(right(), bottom()); }

  bool operator ==(const KDRect &other) const { return (m_origin == other.origin() && m_size == other.size()); }

  void setOrigin(KDPoint origin) { m_origin = origin; }
  void setSize(KDSize size) { m_size = size; }

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
  KDPoint m_origin;
  KDSize m_size;
};

constexpr KDRect KDRectZero = KDRect(0, 0, 0, 0);

#endif
