#ifndef KANDINSKY_SIZE_H
#define KANDINSKY_SIZE_H

#include <kandinsky/coordinate.h>
#include <kandinsky/margins.h>

struct KDSizeStruct {
  KDCoordinate width;
  KDCoordinate height;
};

class KDSize {
 public:
  constexpr KDSize(KDCoordinate width, KDCoordinate height)
      : m_width(width), m_height(height) {}
  constexpr KDSize(KDSizeStruct s) : KDSize(s.width, s.height) {}
  constexpr KDCoordinate width() const { return m_width; }
  constexpr KDCoordinate height() const { return m_height; }
  bool operator==(const KDSize& other) const {
    return m_width == other.width() && m_height == other.height();
  }
  KDSize operator+(const KDSize& other) const {
    return KDSize(m_width + other.width(), m_height + other.height());
  }
  KDSize operator-(const KDSize& other) const {
    return KDSize(m_width - other.width(), m_height - other.height());
  }
  KDSize operator+(const KDMargins& margins) const {
    return KDSize(m_width + margins.width(), m_height + margins.height());
  }
  KDSize operator-(const KDMargins& margins) const {
    return KDSize(m_width - margins.width(), m_height - margins.height());
  }
  constexpr operator KDSizeStruct() const { return {m_width, m_height}; }

 private:
  KDCoordinate m_width;
  KDCoordinate m_height;
};

constexpr KDSize KDSizeZero = KDSize(0, 0);

#endif
