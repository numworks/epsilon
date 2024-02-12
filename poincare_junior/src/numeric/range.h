#ifndef POINCARE_JUNIOR_NUMERIC_RANGE_H
#define POINCARE_JUNIOR_NUMERIC_RANGE_H

#include <assert.h>
#include <omg/ieee754.h>
#include <poincare/coordinate_2D.h>
#include <poincare/float.h>

#include <algorithm>
#include <cmath>

namespace PoincareJ {

template <typename T>
class Range1D {
 public:
  constexpr static T k_defaultHalfLength = static_cast<T>(10.);
  // Ad hoc values
  constexpr static T k_maxFloat = sizeof(T) == sizeof(float) ? 1e35f : 1e300;
  constexpr static T k_minLength = sizeof(T) == sizeof(float) ? 1e-4f : 1e-30;

  static T DefaultLengthAt(T t) {
    return std::max(
        std::pow(static_cast<T>(10.),
                 OMG::IEEE754<T>::exponentBase10(t) - static_cast<T>(2.)),
        k_minLength + Float<T>::EpsilonLax());
  }

  /* Given any two numbers, ValidRangeBetween will return a range with bounds no
   * more than limit, and length no less than k_minLength. */
  static Range1D<T> ValidRangeBetween(T a, T b, T limit = k_maxFloat);

  constexpr Range1D<T>(T min = NAN, T max = NAN, T limit = k_maxFloat)
      : m_min(std::clamp(min, -limit, limit)),
        m_max(std::clamp(max, -limit, limit)) {
    assert(m_min <= m_max || (std::isnan(m_min) && std::isnan(max)));
  }

  bool operator!=(const Range1D<T>& other) const {
    return m_min != other.m_min || m_max != other.m_max;
  }

  bool isNan() const {
    assert(std::isnan(m_min) == std::isnan(m_max));
    return std::isnan(m_min);
  }
  bool isValid() const { return !isNan() && length() >= k_minLength; }
  bool isEmpty() const { return m_min == m_max; }

  T min() const { return m_min; }
  T max() const { return m_max; }

  T length() const { return m_max - m_min; }
  T center() const { return static_cast<T>(0.5) * (m_min + m_max); }

  /* These two methods will prioritize changing the other bound to keep the
   * interval valid.  */
  void setMinKeepingValid(T t, T limit = k_maxFloat) {
    privateSetKeepingValid(t, true, limit);
  }
  void setMaxKeepingValid(T t, T limit = k_maxFloat) {
    privateSetKeepingValid(t, false, limit);
  }

  void extend(T t, T limit);
  void zoom(T ratio, T center);
  void stretchEachBoundBy(T shift, T limit = k_maxFloat);
  void stretchIfTooSmall(T shift = static_cast<T>(-1.), T limit = k_maxFloat);

 private:
  constexpr static T k_zero = static_cast<T>(0.);
  void privateSetKeepingValid(T t, bool isMin, T limit);

  T m_min, m_max;
};

template <typename T>
class Range2D {
 public:
  constexpr Range2D<T>(Range1D<T> x, Range1D<T> y) : m_x(x), m_y(y) {}
  constexpr Range2D<T>(T xMin = NAN, T xMax = NAN, T yMin = NAN, T yMax = NAN)
      : Range2D<T>(Range1D<T>(xMin, xMax), Range1D<T>(yMin, yMax)) {}

  bool operator!=(const Range2D& other) const {
    return m_x != other.m_x || m_y != other.m_y;
  }

  T xMin() const { return m_x.min(); }
  T xMax() const { return m_x.max(); }
  T yMin() const { return m_y.min(); }
  T yMax() const { return m_y.max(); }

  Range1D<T>* x() { return &m_x; }
  Range1D<T>* y() { return &m_y; }
  const Range1D<T>* x() const { return &m_x; }
  const Range1D<T>* y() const { return &m_y; }

  Coordinate2D<T> center() const {
    return Coordinate2D<T>(m_x.center(), m_y.center());
  }
  T ratio() const { return m_y.length() / m_x.length(); }
  bool ratioIs(T r) const;
  void extend(Coordinate2D<T> p, T limit) {
    m_x.extend(p.x(), limit);
    m_y.extend(p.y(), limit);
  }
  void zoom(T ratio, Coordinate2D<T> p) {
    m_x.zoom(ratio, p.x());
    m_y.zoom(ratio, p.y());
  }
  // Return false if failed
  bool setRatio(T r, bool shrink, T limit);

 private:
  static int NormalizationSignificantBits(T xMin, T xMax, T yMin, T yMax);
  Range1D<T> m_x;
  Range1D<T> m_y;
};

}  // namespace PoincareJ

#endif
