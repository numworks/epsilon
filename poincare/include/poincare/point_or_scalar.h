#ifndef POINCARE_POINT_OR_SCALAR_H
#define POINCARE_POINT_OR_SCALAR_H

#include <assert.h>
#include <omg/signaling_nan.h>
#include <poincare/coordinate_2D.h>

namespace Poincare {

/* Using SignalingNan, this structure can store either a scalar or a point. */
template <typename T>
class PointOrRealScalar {
 public:
  PointOrRealScalar(T x, T y) : m_x(x), m_y(y) { assert(isPoint()); }
  PointOrRealScalar(T y) : m_x(OMG::SignalingNan<T>()), m_y(y) {
    assert(isRealScalar());
  }
  bool isRealScalar() const { return OMG::IsSignalingNan(m_x); }
  bool isPoint() const { return !isRealScalar(); }

  T toRealScalar() const {
    assert(isRealScalar());
    return m_y;
  }
  Coordinate2D<T> toPoint() const {
    assert(isPoint());
    return Coordinate2D<T>(m_x, m_y);
  }

 private:
  T m_x;
  T m_y;
};

}  // namespace Poincare

#endif
