#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_MODEL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_MODEL_H

#include "../../shared/curve_view_range.h"
#include "illustrated_list_controller.h"
#include <complex>
#include <poincare/trigonometry.h>

namespace Calculation {

class TrigonometryModel : public Shared::CurveViewRange {
public:
  TrigonometryModel();
  // CurveViewRange
  float xMin() const override { return -k_xHalfRange; }
  float xMax() const override { return k_xHalfRange; }
  float yMin() const override { return yCenter() - yHalfRange(); }
  float yMax() const override { return yCenter() + yHalfRange(); }

  void setAngle(float f) { m_angle = f; }
  float angle() const { return m_angle*(float)M_PI/(float)Poincare::Trigonometry::PiInAngleUnit(Poincare::Preferences::sharedPreferences()->angleUnit()); }
private:
  constexpr static float k_xHalfRange = 2.1f;
  // We center the yRange around the semi-circle where the angle is
  float yCenter() const { return std::sin(angle()) >= 0.0f ? 0.5f : -0.5f; }

  /* We want to normalize the displayed trigonometry circle:
   * - On the X axis, we display 4.4 units on an available pixel width of
   *   (Ion::Display::Width - Escher::Metric::PopUpRightMargin
   *    - Escher::Metric::PopUpLeftMargin)
   * - On the Y axis, the available pixel height is
   *   IllustratedListController::k_illustrationHeight
   */
  float yHalfRange() const;

  float m_angle;
};

}

#endif
