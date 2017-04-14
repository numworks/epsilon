#include "curve_view_cursor.h"
#include <math.h>

namespace Shared {

CurveViewCursor::CurveViewCursor() :
  m_x(NAN),
  m_y(NAN)
{
}

float CurveViewCursor::x() {
  return m_x;
}

float CurveViewCursor::y() {
  return m_y;
}

void CurveViewCursor::moveTo(float x, float y) {
  m_x = clipped(x, false);
  m_y = clipped(y, true);
}

float CurveViewCursor::clipped(float x, bool canBeInfinite) {
  float maxValue = canBeInfinite ? INFINITY : k_maxFloat;
  float clippedX = x > k_maxFloat ? maxValue : x;
  clippedX = clippedX < - k_maxFloat ? -maxValue : clippedX;
  return clippedX;
}

}
