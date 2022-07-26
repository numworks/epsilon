#ifndef CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_MODEL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_MODEL_H

#include "../../shared/curve_view_range.h"
#include "illustrated_expressions_list_controller.h"
#include <algorithm>
#include <complex>
#include <cmath>

namespace Calculation {

class VectorModel : public Shared::CurveViewRange {
public:
  VectorModel();
  // CurveViewRange
  float xMin() const override { return std::min(0.f, vectorX()) - xMargin(); }
  float xMax() const override { return std::max(0.f, vectorX()) + xMargin(); }
  float yMin() const override { return std::min(0.f, vectorY()) - yMargin(); }
  float yMax() const override { return std::max(arcVerticalSpace(), vectorY()) + yMargin(); }

  void setVectorX(float f) { m_vector[0] = f; }
  void setVectorY(float f) { m_vector[1] = f; }
  float vectorX() const { return m_vector[0]; }
  float vectorY() const { return m_vector[1]; }
  float ratio() const { return verticallyCapped() ? height()/k_height : width()/k_width; }

  constexpr static float k_arcRatio = 0.2;
private:
  constexpr static int k_marginInPixels = 12;
  constexpr static int k_width = Ion::Display::Width - Escher::Metric::PopUpRightMargin - Escher::Metric::PopUpLeftMargin - 2 * k_marginInPixels;
  constexpr static int k_height = IllustratedExpressionsListController::k_illustrationHeight - 2 * k_marginInPixels;
  constexpr static float k_xyRatio = static_cast<float>(k_width) / static_cast<float>(k_height);

  /* When the vector points downward, we need to keep space above the origin for
   * the arc which is nearly a circle */
  float arcVerticalSpace() const { return vectorY()<0 ? std::sqrt(m_vector[0]*m_vector[0] + m_vector[1]*m_vector[1]) * k_arcRatio : 0.f; }
  float width() const { return std::abs(vectorX()); }
  float height() const { return std::abs(vectorY()) + arcVerticalSpace(); }

  float xMargin() const {
    return k_marginInPixels*ratio() + (verticallyCapped() ? (height()*k_xyRatio - width()) / 2.f : 0);
  }
  float yMargin() const {
    return k_marginInPixels*ratio() + (verticallyCapped() ? 0 : (width()/k_xyRatio - height()) / 2.f);
  }
  bool verticallyCapped() const {
    return height()*k_xyRatio > width();
  }

  float m_vector[2];
};

}

#endif
