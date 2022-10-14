#ifndef POINCARE_RANGE_H
#define POINCARE_RANGE_H

#include <poincare/coordinate_2D.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

class Range1D {
public:
  constexpr Range1D(float min = NAN, float max = NAN) : m_min(min), m_max(max) { assert(m_min <= m_max || (std::isnan(m_min) && std::isnan(max))); }

  bool isValid() const { assert(std::isnan(m_min) == std::isnan(m_max)); return !std::isnan(m_min); }
  bool isEmpty() const { return m_min == m_max; }
  float & min() { return m_min; }
  float & max() { return m_max; }
  float length() const { return m_max - m_min; }
  float center() const { return 0.5f * (m_min + m_max); }
  void extend(float t);
  void zoom(float ratio, float center);

private:
  float m_min, m_max;
};

class Range2D {
public:
  constexpr Range2D(Range1D x, Range1D y) : m_x(x), m_y(y) {}
  constexpr Range2D(float xMin = NAN, float xMax = NAN, float yMin = NAN, float yMax = NAN) : Range2D(Range1D(xMin, xMax), Range1D(yMin, yMax)) {}

  Range1D & x() { return m_x; }
  Range1D & y() { return m_y; }
  Coordinate2D<float> center() const { return Coordinate2D<float>(m_x.center(), m_y.center()); }
  void extend(Coordinate2D<float> p) { m_x.extend(p.x1()); m_y.extend(p.x2()); }
  void zoom(float ratio, Coordinate2D<float> p) { m_x.zoom(ratio, p.x1()); m_y.zoom(ratio, p.x2()); }

private:
  Range1D m_x;
  Range1D m_y;
};

}

#endif
