#ifndef APPS_CURVE_VIEW_WINDOW_WITH_CURSOR_H
#define APPS_CURVE_VIEW_WINDOW_WITH_CURSOR_H

#include "curve_view_window.h"

class CurveViewWindowWithCursor : public CurveViewWindow {
public:
  CurveViewWindowWithCursor();
  float xCursorPosition();
  float yCursorPosition();
protected:
  float m_xCursorPosition;
  float m_yCursorPosition;
};

#endif
