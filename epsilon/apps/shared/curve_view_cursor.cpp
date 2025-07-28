#include "curve_view_cursor.h"

#include <assert.h>

#include <cmath>

namespace Shared {

CurveViewCursor::CurveViewCursor(double maxValue)
    : m_t(NAN), m_x(NAN), m_y(NAN), m_maxValue(maxValue) {}

void CurveViewCursor::moveTo(double t, double x, double y) {
  assert(!std::isnan(t));
  m_t = clipped(t, false);
  m_x = clipped(x, true);
  m_y = clipped(y, true);
}

double CurveViewCursor::clipped(double x, bool canBeInfinite) {
  double maxValue = canBeInfinite ? INFINITY : m_maxValue;
  if (x > m_maxValue) {
    return maxValue;
  }
  if (x < -m_maxValue) {
    return -maxValue;
  }
  return x;
}

}  // namespace Shared
