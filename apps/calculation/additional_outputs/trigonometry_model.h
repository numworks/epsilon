#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_MODEL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_MODEL_H

#include <escher/metric.h>
#include <apps/shared/curve_view_range.h>

namespace Calculation {

/*  We want the circles to fall exactly on pixels for the graph to be sharp.
    To match the mockup these integral values are the best :

    px -> math
     2 -> -2
    67 -> -1
   132 ->  0
   197 ->  1
   262 ->  2

   Since the curve view is 264 pixels wide we need to set :
   xMax - xMin = 264 * (2 - (-2)) / (262 - 2)
*/

class TrigonometryModel : public Shared::CurveViewRange {
public:
  TrigonometryModel() : Shared::CurveViewRange(), m_angle(NAN) {}

  // CurveViewRange
  // xMin should be 2 pixels left to the point -2.0
  float xMin() const override { return -2.f - 2 * k_targetRatio; }
  float xMax() const override { return xMin() + xRange(); }
  float yMin() const override { return yCenter() - yRange() / 2.f; }
  float yMax() const override { return yCenter() + yRange() / 2.f; }

  float angle() const { return m_angle; }
  void setAngle(float f) { m_angle = f; }

private:
  constexpr static float k_targetRatio = 4.f / 260.f; // see above
  constexpr static float k_width =  Ion::Display::Width - Escher::Metric::PopUpRightMargin - Escher::Metric::PopUpLeftMargin - Escher::Metric::CellSeparatorThickness * 2;
  static_assert(k_width == 264, "Trigonometry model is built with the assumption that the graph view is 264 pixels wide.");

  // We center the yRange around the semi-circle where the angle is
  float yCenter() const { return std::sin(angle()) >= 0.0f ? 0.5f : -0.5f; }
  // -1 to reflect the -1 in PlotView::graphWidth
  float xRange() const { return (k_width - 1) * k_targetRatio; }
  float yRange() const;

  float m_angle;
};

}

#endif
