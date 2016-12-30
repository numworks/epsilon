#include "curve_view_window_with_cursor.h"
#include <math.h>

CurveViewWindowWithCursor::CurveViewWindowWithCursor() :
  m_xCursorPosition(NAN),
  m_yCursorPosition(NAN)
{
}

float CurveViewWindowWithCursor::xCursorPosition() {
  return m_xCursorPosition;
}

float CurveViewWindowWithCursor::yCursorPosition() {
  return m_yCursorPosition;
}
