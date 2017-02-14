#include "curve_view_cursor.h"
#include <math.h>

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
  m_x = x;
  m_y = y;
}
