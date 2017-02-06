#ifndef APPS_INTERACTIVE_CURVE_VIEW_RANGE_H
#define APPS_INTERACTIVE_CURVE_VIEW_RANGE_H

#include <stdint.h>
#include <poincare.h>
#include "memoized_curve_view_range.h"
#include "curve_view_cursor.h"
#include "interactive_curve_view_range_delegate.h"

class InteractiveCurveViewRange : public MemoizedCurveViewRange {
public:
  InteractiveCurveViewRange(CurveViewCursor * cursor, InteractiveCurveViewRangeDelegate * delegate);
  void setCursor(CurveViewCursor * cursor);
  uint32_t rangeChecksum() override;

  //CurveViewWindow
  bool yAuto();
  void setXMin(float f) override;
  void setXMax(float f) override;
  void setYAuto(bool yAuto);

  // Window
  void zoom(float ratio);
  void panWithVector(float x, float y);
  void roundAbscissa();
  void normalize();
  void setTrigonometric(Poincare::Expression::AngleUnit angleUnit);
  virtual void setDefault();
  void centerAxisAround(Axis axis, float position);
  void panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRation, float leftMarginRation);
protected:
  bool m_yAuto;
private:
  CurveViewCursor * m_cursor;
  InteractiveCurveViewRangeDelegate * m_delegate;
};

typedef void (InteractiveCurveViewRange::*ParameterSetterPointer)(float);
typedef float (InteractiveCurveViewRange::*ParameterGetterPointer)();
typedef void (InteractiveCurveViewRange::*RangeMethodPointer)();

#endif
