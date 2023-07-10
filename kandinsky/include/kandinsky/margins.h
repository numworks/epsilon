#ifndef KANDINSKY_MARGIN_H
#define KANDINSKY_MARGIN_H

#include <kandinsky/coordinate.h>
#include <kandinsky/point.h>

class KD1DMargins {
 public:
  constexpr KD1DMargins(KDCoordinate firstMargin, KDCoordinate secondMargin)
      : m_firstMargin(firstMargin), m_secondMargin(secondMargin) {}
  constexpr KD1DMargins(KDCoordinate margin)
      : m_firstMargin(margin), m_secondMargin(margin) {}
  constexpr KD1DMargins() : m_firstMargin(0), m_secondMargin(0) {}

 protected:
  constexpr KDCoordinate firstMargin() const { return m_firstMargin; }
  constexpr KDCoordinate secondMargin() const { return m_secondMargin; }

  void setFirstMargin(KDCoordinate m) { m_firstMargin = m; }
  void setSecondMargin(KDCoordinate m) { m_secondMargin = m; }

  constexpr KDCoordinate total() const {
    return m_firstMargin + m_secondMargin;
  }

 private:
  KDCoordinate m_firstMargin;
  KDCoordinate m_secondMargin;
};

// KDHorizontalMargins(left, right)
// KDHorizontalMargins(margin) -> both margins are equal
// KDHorizontalMargins() -> both margins are 0
class KDHorizontalMargins : public KD1DMargins {
 public:
  using KD1DMargins::KD1DMargins;

  constexpr bool operator==(const KDHorizontalMargins& other) {
    return other.left() == left() && other.right() == right();
  }

  constexpr KDCoordinate left() const { return firstMargin(); }
  constexpr KDCoordinate right() const { return secondMargin(); }

  void setLeft(KDCoordinate m) { setFirstMargin(m); }
  void setRight(KDCoordinate m) { setSecondMargin(m); }

  constexpr KDCoordinate width() const { return total(); }
};

// KDVerticalMargins(top, bottom)
// KDVerticalMargins(margin) -> both margins are equal
// KDVerticalMargins() -> both margins are 0
class KDVerticalMargins : public KD1DMargins {
 public:
  using KD1DMargins::KD1DMargins;

  constexpr bool operator==(const KDVerticalMargins& other) {
    return other.top() == top() && other.bottom() == bottom();
  }

  constexpr KDCoordinate top() const { return firstMargin(); }
  constexpr KDCoordinate bottom() const { return secondMargin(); }

  void setTop(KDCoordinate m) { setFirstMargin(m); }
  void setBottom(KDCoordinate m) { setSecondMargin(m); }

  constexpr KDCoordinate height() const { return total(); }
};

// KDMargins(left, right, top, bottom)
// KDMargins(horizontal, vertical)
// KDMargins(margin) -> all margins are equal
// KDMargins() -> all margins are 0
class KDMargins {
 public:
  constexpr KDMargins(KDCoordinate left, KDCoordinate right, KDCoordinate top,
                      KDCoordinate bottom)
      : m_horizontal(left, right), m_vertical(top, bottom) {}
  constexpr KDMargins(KDHorizontalMargins horizontal,
                      KDVerticalMargins vertical)
      : m_horizontal(horizontal), m_vertical(vertical) {}
  constexpr KDMargins(KDCoordinate margin)
      : m_horizontal(margin), m_vertical(margin) {}
  constexpr KDMargins() : m_horizontal(), m_vertical() {}

  constexpr bool operator==(const KDMargins& other) {
    return other.horizontal() == horizontal() && other.vertical() == vertical();
  }
  // Unary minus
  constexpr KDMargins operator-() const {
    return KDMargins(-left(), -right(), -top(), -bottom());
  }

  constexpr KDHorizontalMargins horizontal() const { return m_horizontal; }
  constexpr KDVerticalMargins vertical() const { return m_vertical; }

  constexpr KDCoordinate left() const { return m_horizontal.left(); }
  constexpr KDCoordinate right() const { return m_horizontal.right(); }
  constexpr KDCoordinate top() const { return m_vertical.top(); }
  constexpr KDCoordinate bottom() const { return m_vertical.bottom(); }

  void setHorizontal(KDHorizontalMargins m) { m_horizontal = m; }
  void setVertical(KDVerticalMargins m) { m_vertical = m; }

  void setLeft(KDCoordinate m) { m_horizontal.setLeft(m); }
  void setRight(KDCoordinate m) { m_horizontal.setRight(m); }
  void setTop(KDCoordinate m) { m_vertical.setTop(m); }
  void setBottom(KDCoordinate m) { m_vertical.setBottom(m); }

  constexpr KDCoordinate width() const { return m_horizontal.width(); }
  constexpr KDCoordinate height() const { return m_vertical.height(); }
  constexpr KDPoint leftTopPoint() const { return KDPoint(left(), top()); }

 private:
  KDHorizontalMargins m_horizontal;
  KDVerticalMargins m_vertical;
};

#endif
