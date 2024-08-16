#ifndef KANDINSKY_POINT_H
#define KANDINSKY_POINT_H

#include <kandinsky/coordinate.h>

struct KDPointStruct {
  KDCoordinate x;
  KDCoordinate y;
};

class KDPoint {
 public:
  constexpr KDPoint(KDCoordinate x, KDCoordinate y) : m_struct{x, y} {}
  constexpr KDPoint(KDPointStruct p) : m_struct{p} {}
  KDCoordinate x() const { return m_struct.x; }
  KDCoordinate y() const { return m_struct.y; }
  KDPoint translatedBy(KDPoint other) const;
  KDPoint relativeTo(KDPoint p) const { return translatedBy(p.opposite()); }
  KDPoint opposite() const;
  bool operator==(const KDPoint& other) const {
    return m_struct.x == other.m_struct.x && m_struct.y == other.m_struct.y;
  }
  bool operator!=(const KDPoint& other) const { return !(operator==(other)); }
  constexpr operator KDPointStruct() const { return m_struct; }
  uint16_t squareDistanceTo(KDPoint other) const;

 private:
  KDPointStruct m_struct;
};

constexpr KDPoint KDPointZero = KDPoint(0, 0);

#endif
