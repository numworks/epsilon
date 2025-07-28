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
      : m_struct{width, height} {}
  constexpr KDSize(KDSizeStruct s) : m_struct{s} {}
  constexpr KDCoordinate width() const { return m_struct.width; }
  constexpr KDCoordinate height() const { return m_struct.height; }
  bool operator==(const KDSize& other) const {
    return m_struct.width == other.m_struct.width &&
           m_struct.height == other.m_struct.height;
  }
  KDSize operator+(const KDSize& other) const {
    return KDSize(width() + other.width(), height() + other.height());
  }
  KDSize operator-(const KDSize& other) const {
    return KDSize(width() - other.width(), height() - other.height());
  }
  KDSize operator+(const KDMargins& margins) const {
    return KDSize(width() + margins.width(), height() + margins.height());
  }
  KDSize operator-(const KDMargins& margins) const {
    return KDSize(width() - margins.width(), height() - margins.height());
  }
  constexpr operator KDSizeStruct() const { return m_struct; }

 private:
  KDSizeStruct m_struct;
};

constexpr KDSize KDSizeZero = KDSize(0, 0);

#endif
