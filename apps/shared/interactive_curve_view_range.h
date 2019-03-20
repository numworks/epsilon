#ifndef SHARED_INTERACTIVE_CURVE_VIEW_RANGE_H
#define SHARED_INTERACTIVE_CURVE_VIEW_RANGE_H

#include <stdint.h>
#include "memoized_curve_view_range.h"
#include "interactive_curve_view_range_delegate.h"
#include <ion/display.h>
#include <float.h>

namespace Shared {

class InteractiveCurveViewRange : public MemoizedCurveViewRange {
public:
  constexpr static float k_minFloat = 1E-4f;
  InteractiveCurveViewRange(InteractiveCurveViewRangeDelegate * delegate = nullptr) :
    MemoizedCurveViewRange(),
    m_yAuto(true),
    m_delegate(delegate)
  {}

  void setDelegate(InteractiveCurveViewRangeDelegate * delegate) { m_delegate = delegate; }
  uint32_t rangeChecksum() override;

  bool yAuto() const { return m_yAuto; }
  void setYAuto(bool yAuto);

  // CurveViewWindow
  void setXMin(float f) override;
  void setXMax(float f) override;
  void setYMin(float f) override;
  void setYMax(float f) override;

  // Window
  void zoom(float ratio, float x, float y);
  void panWithVector(float x, float y);
  virtual void roundAbscissa();
  virtual void normalize();
  virtual void setTrigonometric();
  virtual void setDefault();
  void centerAxisAround(Axis axis, float position);
  void panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRation, float leftMarginRation);
protected:
  bool m_yAuto;
  /* In normalized settings, we put each axis so that 1cm = 2 units. For now,
   * the screen has size 43.2mm * 57.6mm.
   * We want:
   *   2*NormalizedXHalfRange -> 57.6mm
   *   2*1 -> 10.0mm
   * So NormalizedXHalfRange = 5.76
   * We want:
   *   2*NormalizedYHalfRange -> 43.2mm * 170/240
   *   2*1 -> 10.0mm
   * So NormalizedYHalfRange = 3.06 */
  constexpr static float NormalizedXHalfRange() { return 5.76f; }
  constexpr static float NormalizedYHalfRange() { return 3.06f; }
  static float clipped(float f, bool isMax);
  InteractiveCurveViewRangeDelegate * m_delegate;
private:
  constexpr static float k_upperMaxFloat = 1E+8f;
  constexpr static float k_lowerMaxFloat = 9E+7f;
  constexpr static float k_maxRatioPositionRange = 1E5f;
  void notifyRangeChange();
};

static_assert(InteractiveCurveViewRange::k_minFloat >= FLT_EPSILON, "InteractiveCurveViewRange's minimal float range is lower than float precision, it might draw uglily curves such as cos(x)^2+sin(x)^2");
static_assert(Ion::Display::WidthInTenthOfMillimeter == 576, "Use the new screen width to compute Shared::InteractiveCurveViewRange::NormalizedXHalfRange");
static_assert(Ion::Display::HeightInTenthOfMillimeter == 432, "Use the new screen height to compute Shared::InteractiveCurveViewRange::NormalizedYHalfRange");

typedef void (InteractiveCurveViewRange::*ParameterSetterPointer)(float);
typedef float (InteractiveCurveViewRange::*ParameterGetterPointer)();
typedef void (InteractiveCurveViewRange::*RangeMethodPointer)();

}

#endif
