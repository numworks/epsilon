#ifndef POINCARE_RANGE_H
#define POINCARE_RANGE_H

#include <poincare/coordinate_2D.h>
#include <poincare/ieee754.h>
#include <assert.h>
#include <algorithm>
#include <cmath>

namespace Poincare {

class Range1D {
public:
  constexpr static float k_maxFloat = 1e35f;
  constexpr static float k_minLength = 1e-4f;
  constexpr static float k_defaultHalfLength = 10.f;

  static float DefaultLengthAt(float t) { return std::pow(10.f, IEEE754<float>::exponentBase10(t) - 1.f); }
  /* Given any two numbers, RangeBetween will return a range with bounds no
   * more than limit, and length no less than k_minLength. */
  static Range1D RangeBetween(float a, float b, float limit = k_maxFloat);

  constexpr Range1D(float min = NAN, float max = NAN) : m_min(min), m_max(max) { assert(m_min <= m_max || (std::isnan(m_min) && std::isnan(max))); }

  bool isValid() const { assert(std::isnan(m_min) == std::isnan(m_max)); return !std::isnan(m_min); }
  bool isEmpty() const { return m_min == m_max; }

  float min() const { return m_min; }
  float max() const { return m_max; }
  void setMin(float t, float limit = k_maxFloat) { privateSet(t, true, limit); }
  void setMax(float t, float limit = k_maxFloat) { privateSet(t, false, limit); }

  float length() const { return m_max - m_min; }
  float center() const { return 0.5f * (m_min + m_max); }
  void extend(float t);
  void zoom(float ratio, float center);

private:
  void privateSet(float t, bool isMin, float limit);

  float m_min, m_max;
};

class Range2D {
public:
  constexpr Range2D(Range1D x, Range1D y) : m_x(x), m_y(y) {}
  constexpr Range2D(float xMin = NAN, float xMax = NAN, float yMin = NAN, float yMax = NAN) : Range2D(Range1D(xMin, xMax), Range1D(yMin, yMax)) {}

  float xMin() const { return m_x.min(); }
  float xMax() const { return m_x.max(); }
  float yMin() const { return m_y.min(); }
  float yMax() const { return m_y.max(); }

  Range1D * x() { return &m_x; }
  Range1D * y() { return &m_y; }
  const Range1D * x() const { return &m_x; }
  const Range1D * y() const { return &m_y; }

  Coordinate2D<float> center() const { return Coordinate2D<float>(m_x.center(), m_y.center()); }
  float ratio() const { return m_y.length() / m_x.length(); }
  bool ratioIs(float r) const;
  void extend(Coordinate2D<float> p) { m_x.extend(p.x1()); m_y.extend(p.x2()); }
  void zoom(float ratio, Coordinate2D<float> p) { m_x.zoom(ratio, p.x1()); m_y.zoom(ratio, p.x2()); }
  void setRatio(float r, bool shrink);

private:
  Range1D m_x;
  Range1D m_y;
};

}

#endif
