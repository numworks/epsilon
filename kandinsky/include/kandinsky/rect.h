#ifndef KANDINSKY_RECT_H
#define KANDINSKY_RECT_H

#include <kandinsky/coordinate.h>
#include <kandinsky/point.h>
#include <kandinsky/size.h>

/*   +-+-+-+-+-+
 *   |x| | | | |
 *   +-+-+-+-+-+
 *   | | | | | |
 *   +-+-+-+-+-+
 *   | | | | |o|
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

struct KDRectStruct {
  KDPointStruct origin;
  KDSizeStruct size;
};

class KDRect {
 public:
  constexpr KDRect(KDCoordinate x, KDCoordinate y, KDCoordinate width,
                   KDCoordinate height)
      : m_struct{{x, y}, {width, height}} {}
  constexpr KDRect(KDRectStruct r) : m_struct{r} {}
  constexpr KDRect(KDPoint p, KDSize s) : m_struct{p, s} {}
  constexpr KDRect(KDCoordinate x, KDCoordinate y, KDSize s)
      : m_struct{{x, y}, s} {}
  constexpr KDRect(KDPoint p, KDCoordinate width, KDCoordinate height)
      : m_struct{p, {width, height}} {}

  constexpr KDCoordinate x() const { return origin().x(); }
  constexpr KDCoordinate y() const { return origin().y(); }
  constexpr KDPoint origin() const { return m_struct.origin; }
  constexpr KDCoordinate width() const { return size().width(); }
  constexpr KDCoordinate height() const { return size().height(); }
  constexpr KDSize size() const { return m_struct.size; }
  constexpr KDCoordinate top() const { return y(); }
  constexpr KDCoordinate right() const { return x() + width() - 1; }
  constexpr KDCoordinate bottom() const { return y() + height() - 1; }
  constexpr KDCoordinate left() const { return x(); }

  constexpr KDPoint topLeft() const { return KDPoint(left(), top()); }
  constexpr KDPoint topRight() const { return KDPoint(right(), top()); }
  constexpr KDPoint bottomLeft() const { return KDPoint(left(), bottom()); }
  constexpr KDPoint bottomRight() const { return KDPoint(right(), bottom()); }

  bool operator==(const KDRect& other) const {
    return (origin() == other.origin() && size() == other.size());
  }
  bool operator!=(const KDRect& other) const { return !(other == *this); }
  constexpr operator KDRectStruct() const { return m_struct; }

  void setOrigin(KDPoint origin) { m_struct.origin = origin; }
  void setSize(KDSize size) { m_struct.size = size; }

  KDRect transposed() const;
  KDRect translatedBy(KDPoint p) const;
  KDRect relativeTo(KDPoint p) const { return translatedBy(p.opposite()); }
  KDRect paddedWith(KDCoordinate value) const;
  KDRect trimmedBy(KDCoordinate value) const { return paddedWith(-value); }
  constexpr KDRect movedTo(KDPoint p) const { return KDRect(p, size()); }
  bool intersects(const KDRect& other) const;
  KDRect intersectedWith(const KDRect& other) const;
  // Returns the smallest rectangle containing r1 and r2
  KDRect unionedWith(const KDRect& other) const;
  // Returns the smallest rectangle containing r1\r2
  KDRect differencedWith(const KDRect& other) const;
  bool contains(KDPoint p) const;
  bool containsRect(const KDRect& other) const;
  bool isAbove(KDPoint p) const;
  bool isUnder(KDPoint p) const;
  bool isEmpty() const;

 private:
  KDRectStruct m_struct;
};

constexpr KDRect KDRectZero = KDRect(0, 0, 0, 0);

#endif
