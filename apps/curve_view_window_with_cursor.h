#ifndef APPS_CURVE_VIEW_WINDOW_WITH_CURSOR_H
#define APPS_CURVE_VIEW_WINDOW_WITH_CURSOR_H

#include "curve_view_window.h"

class CurveViewWindowWithCursor : public CurveViewWindow {
public:
  enum class Direction {
    Up,
    Left,
    Down,
    Right
  };
  CurveViewWindowWithCursor();
  // Cursor
  float xCursorPosition();
  float yCursorPosition();
  virtual int moveCursorHorizontally(int direction) = 0;
  virtual int moveCursorVertically(int direction) = 0;

  //CurveViewWindow
  float xMin() override;
  float xMax() override;
  float yMin() override;
  float yMax() override;
  bool yAuto();
  float xGridUnit() override;
  float yGridUnit() override;
  void setXMin(float f);
  void setXMax(float f);
  void setYMin(float f);
  void setYMax(float f);
  void setYAuto(bool yAuto);

  // Window
  void zoom(float ratio);
  void translateWindow(Direction direction);
  void roundAbscissa();
  void normalize();
  void setDefault();
protected:
  constexpr static float k_cursorMarginFactorToBorder = 0.025f;
  constexpr static float k_numberOfCursorStepsInGradUnit = 5.0f;
  bool panToMakePointVisible(float x, float y, float xMargin, float yMargin);
  void centerAxisAround(Axis axis, float position);
  virtual void initCursorPosition() = 0;
  virtual bool computeYaxis() = 0;

  // Cursor
  float m_xCursorPosition;
  float m_yCursorPosition;

  // Window bounds of the data
  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;
  bool m_yAuto;
  float m_xGridUnit;
  float m_yGridUnit;
};

#endif
