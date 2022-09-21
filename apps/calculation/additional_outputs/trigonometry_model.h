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
  float xMin() const override { return -2 - 2*(4.f/260.f); }
  float xMax() const override { return xRange() + xMin(); }
  float yMin() const override { return yCenter() - yHalfRange(); }
  float yMax() const override { return yCenter() + yHalfRange(); }

  float xRange() const { return 4.f * 263.f/ 260.f; }
  float yRange() const;

  void setAngle(float f) { m_angle = f; }
  float angle() const { return m_angle*(float)M_PI/(float)Poincare::Trigonometry::PiInAngleUnit(Poincare::Preferences::sharedPreferences()->angleUnit()); }
private:
  constexpr static KDCoordinate width =  Ion::Display::Width - Escher::Metric::PopUpRightMargin - Escher::Metric::PopUpLeftMargin - 2;
  constexpr static KDCoordinate halfWidth = width / 2;
  constexpr static float k_xHalfRange = 2;//(float)width / (halfWidth + 1.f);
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

/* The curve view is 264 pixels wide
   we want to fall exactly on pixels when drawing integral points
   mockup
   1 -> -2
   69 -> -1
   132 -> 0
   195 -> 1
   263 -> 2

   1 -> -2
   66 -> 1
   131 -> 0
   196 -> 1
   261 -> 2

   4 -> 260
   xMax - xMin = 4*264/260
*/
#endif
