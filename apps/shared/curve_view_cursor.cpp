#include "curve_view_cursor.h"
#include <cmath>

namespace Shared {

CurveViewCursor::CurveViewCursor() :
  m_x(NAN),
  m_y(NAN)
{
}

double CurveViewCursor::x() {
  return m_x;
}

double CurveViewCursor::y() {
  return m_y;
}

void CurveViewCursor::moveTo(double x, double y) {
  m_x = clipped(x, false);
  m_y = clipped(y, true);
}

double CurveViewCursor::clipped(double x, bool canBeInfinite) {
  double maxValue = canBeInfinite ? INFINITY : k_maxFloat;
  double clippedX = x > k_maxFloat ? maxValue : x;
  clippedX = clippedX < - k_maxFloat ? -maxValue : clippedX;
  return clippedX;
}

}
