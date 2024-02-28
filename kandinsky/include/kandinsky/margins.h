#ifndef KANDINSKY_MARGIN_H
#define KANDINSKY_MARGIN_H

#include <kandinsky/coordinate.h>
#include <kandinsky/point.h>

class KD1DMargins {
 public:
  constexpr KD1DMargins(KDCoordinate firstMargin, KDCoordinate secondMargin)
      : m_firstMargin(firstMargin), m_secondMargin(secondMargin) {}
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
// KDHorizontalMargins() -> both margins are 0
class KDHorizontalMargins : public KD1DMargins {
 public:
  using KD1DMargins::KD1DMargins;

  constexpr bool operator==(const KDHorizontalMargins& other) const {
    return other.left() == left() && other.right() == right();
  }

  constexpr KDCoordinate left() const { return firstMargin(); }
  constexpr KDCoordinate right() const { return secondMargin(); }

  void setLeft(KDCoordinate m) { setFirstMargin(m); }
  void setRight(KDCoordinate m) { setSecondMargin(m); }

  constexpr KDCoordinate width() const { return total(); }
};

// KDVerticalMargins(top, bottom)
// KDVerticalMargins() -> both margins are 0
class KDVerticalMargins : public KD1DMargins {
 public:
  using KD1DMargins::KD1DMargins;

  constexpr bool operator==(const KDVerticalMargins& other) const {
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
class KDMargins : public KDHorizontalMargins, public KDVerticalMargins {
 public:
  constexpr KDMargins(KDCoordinate left, KDCoordinate right, KDCoordinate top,
                      KDCoordinate bottom)
      : KDHorizontalMargins(left, right), KDVerticalMargins(top, bottom) {}
  constexpr KDMargins(KDHorizontalMargins horizontal,
                      KDVerticalMargins vertical)
      : KDHorizontalMargins(horizontal), KDVerticalMargins(vertical) {}
  constexpr explicit KDMargins(KDCoordinate margin)
      : KDHorizontalMargins(margin, margin),
        KDVerticalMargins(margin, margin) {}
  constexpr KDMargins() : KDHorizontalMargins(), KDVerticalMargins() {}

  constexpr bool operator==(const KDMargins& other) const {
    return other.horizontal() == horizontal() && other.vertical() == vertical();
  }
  // Unary minus
  constexpr KDMargins operator-() const {
    return KDMargins(-left(), -right(), -top(), -bottom());
  }

  constexpr KDHorizontalMargins horizontal() const { return *this; }
  constexpr KDVerticalMargins vertical() const { return *this; }

  void setHorizontal(KDHorizontalMargins m) {
    static_cast<KDHorizontalMargins&>(*this) = m;
  }
  void setVertical(KDVerticalMargins m) {
    static_cast<KDVerticalMargins&>(*this) = m;
  }

  constexpr KDPoint topLeftPoint() const { return KDPoint(left(), top()); }
};

#endif
