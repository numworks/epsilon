#include "curve_view_cursor.h"
#include <cmath>

namespace Shared {

CurveViewCursor::CurveViewCursor() : m_t(NAN), m_x(NAN), m_y(NAN) {}

void CurveViewCursor::moveTo(double t, double x, double y) {
  m_t = clipped(t, false);
  m_x = clipped(x, false); //TODO LEA ?
  m_y = clipped(y, true);
}

double CurveViewCursor::clipped(double x, bool canBeInfinite) {
  double maxValue = canBeInfinite ? INFINITY : k_maxFloat;
  if (x > k_maxFloat) {
    return maxValue;
  }
  if (x < -k_maxFloat) {
    return -maxValue;
  }
  return x;
}

}
