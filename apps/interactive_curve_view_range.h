#ifndef APPS_INTERACTIVE_CURVE_VIEW_RANGE_H
#define APPS_INTERACTIVE_CURVE_VIEW_RANGE_H

#include <stdint.h>
#include "curve_view_range.h"
#include "curve_view_cursor.h"
#include "interactive_curve_view_range_delegate.h"

class InteractiveCurveViewRange : public CurveViewRange {
public:
  InteractiveCurveViewRange(CurveViewCursor * cursor, InteractiveCurveViewRangeDelegate * delegate);
  void setCursor(CurveViewCursor * cursor);
  uint32_t rangeChecksum();

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
  void panWithVector(float x, float y);
  void roundAbscissa();
  void normalize();
  void setTrigonometric();
  virtual void setDefault();
  void centerAxisAround(Axis axis, float position);
  
  //Cursor
  // moveCursorTo returns true if the window has moved
  bool moveCursorTo(float x, float y);
  constexpr static float k_numberOfCursorStepsInGradUnit = 5.0f;
protected:
  constexpr static float k_cursorMarginFactorToBorder = 0.025f;
  bool panToMakePointVisible(float x, float y, float xMargin, float yMargin);
  // Window bounds of the data
  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;
  bool m_yAuto;
  float m_xGridUnit;
  float m_yGridUnit;
  CurveViewCursor * m_cursor;
  InteractiveCurveViewRangeDelegate * m_delegate;
};

typedef void (InteractiveCurveViewRange::*ParameterSetterPointer)(float);
typedef float (InteractiveCurveViewRange::*ParameterGetterPointer)();
typedef void (InteractiveCurveViewRange::*RangeMethodPointer)();

#endif
