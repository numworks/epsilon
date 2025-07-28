#ifndef CALCULATION_VECTOR_MODEL_H
#define CALCULATION_VECTOR_MODEL_H

#include <apps/shared/memoized_curve_view_range.h>

#include "illustrated_expressions_list_controller.h"

namespace Calculation {

class VectorModel : public Shared::MemoizedCurveViewRange {
 public:
  VectorModel()
      : Shared::MemoizedCurveViewRange(), m_vector{NAN, NAN}, m_angle(NAN) {}

  void setVector(float x, float y);
  void setAngle(float angle) {
    assert(std::isfinite(angle));
    assert(0 <= angle && angle < 2 * M_PI + OMG::Float::EpsilonLax<float>());
    m_angle = angle;
  }
  float vectorX() const { return m_vector[0]; }
  float vectorY() const { return m_vector[1]; }
  float angle() const { return m_angle; }

 private:
  // Margin used to separate the drawings from the curve view's border
  constexpr static int k_marginInPixels = 12;
  constexpr static int k_width = Ion::Display::Width -
                                 Escher::Metric::PopUpMargins.width() -
                                 2 * k_marginInPixels;
  constexpr static int k_height =
      IllustratedExpressionsListController::k_illustrationHeight -
      2 * k_marginInPixels;
  constexpr static float k_xyRatio =
      static_cast<float>(k_width) / static_cast<float>(k_height);
  void recomputeViewRange();
  float m_vector[2];
  float m_angle;
};

}  // namespace Calculation

#endif
