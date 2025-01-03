#ifndef KANDINSKY_MARGIN_H
#define KANDINSKY_MARGIN_H

#include <kandinsky/coordinate.h>
#include <kandinsky/point.h>

struct KD1DMarginsStruct {
  KDCoordinate firstMargin;
  KDCoordinate secondMargin;
};

struct KDMarginsStruct {
  KD1DMarginsStruct horizontal;
  KD1DMarginsStruct vertical;
};

class KD1DMargins {
 public:
  constexpr KD1DMargins(KDCoordinate firstMargin, KDCoordinate secondMargin)
      : m_struct{firstMargin, secondMargin} {}
  constexpr KD1DMargins(KD1DMarginsStruct margins) : m_struct{margins} {}
  constexpr KD1DMargins() : m_struct{0, 0} {}

  constexpr operator KD1DMarginsStruct() const { return m_struct; }

 protected:
  constexpr KDCoordinate firstMargin() const { return m_struct.firstMargin; }
  constexpr KDCoordinate secondMargin() const { return m_struct.secondMargin; }

  void setFirstMargin(KDCoordinate m) { m_struct.firstMargin = m; }
  void setSecondMargin(KDCoordinate m) { m_struct.secondMargin = m; }

  constexpr KDCoordinate total() const {
    return firstMargin() + secondMargin();
  }

 private:
  KD1DMarginsStruct m_struct;
};

// KDHorizontalMargins(left, right)
// KDHorizontalMargins() -> both margins are 0
class KDHorizontalMargins : public KD1DMargins {
 public:
  using KD1DMargins::KD1DMargins;

  constexpr bool operator==(const KDHorizontalMargins& other) const {
    return other.left() == left() && other.right() == right();
  }

  constexpr operator KDMarginsStruct() const {
    return {{left(), right()}, {0, 0}};
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

  constexpr operator KDMarginsStruct() const {
    return {{0, 0}, {top(), bottom()}};
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
  constexpr KDMargins(KDMarginsStruct margins)
      : KDMargins(margins.horizontal, margins.vertical) {}

  constexpr bool operator==(const KDMargins& other) const {
    return other.horizontal() == horizontal() && other.vertical() == vertical();
  }
  // Unary minus
  constexpr KDMargins operator-() const {
    return KDMargins(-left(), -right(), -top(), -bottom());
  }
  constexpr operator KDMarginsStruct() const {
    return {horizontal(), vertical()};
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
