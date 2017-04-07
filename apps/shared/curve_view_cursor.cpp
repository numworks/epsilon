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
  m_x = clipped(x);
  m_y = clipped(y);
}

float CurveViewCursor::clipped(float x) {
  float clippedX = x > k_maxFloat ? INFINITY : x;
  clippedX = clippedX < - k_maxFloat ? -INFINITY : clippedX;
  return clippedX;
}

}
