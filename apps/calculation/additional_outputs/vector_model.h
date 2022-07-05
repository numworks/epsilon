#ifndef CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_MODEL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_MODEL_H

#include "../../shared/curve_view_range.h"
#include "illustrated_list_controller.h"
#include <algorithm>
#include <complex>

namespace Calculation {

class VectorModel : public Shared::CurveViewRange {
public:
  VectorModel();
  // CurveViewRange
  float xMin() const override { return std::min(0.f, vectorX()) - xMargin(); }
  float xMax() const override { return std::max(0.f, vectorX()) + xMargin(); }
  float yMin() const override { return std::min(0.f, vectorY()) - yMargin(); }
  float yMax() const override { return std::max(0.f, vectorY()) + yMargin(); }

  void setVectorX(float f) { m_vector[0] = f; }
  void setVectorY(float f) { m_vector[1] = f; }
  float vectorX() const { return m_vector[0]; }
  float vectorY() const { return m_vector[1]; }
  float ratio() const {
    return verticallyCapped() ? std::abs(vectorY())/k_height : std::abs(vectorX())/k_width;
  }
private:
  constexpr static int k_marginInPixels = 12;
  constexpr static int k_width = Ion::Display::Width - Escher::Metric::PopUpRightMargin - Escher::Metric::PopUpLeftMargin - 2 * k_marginInPixels;
  constexpr static int k_height = IllustratedListController::k_illustrationHeight - 2 * k_marginInPixels;
  constexpr static float k_xyRatio = static_cast<float>(k_width) / static_cast<float>(k_height);
  float xMargin() const {
    return k_marginInPixels*ratio() + (verticallyCapped() ? (std::abs(vectorY())*k_xyRatio - std::abs(vectorX())) / 2.f : 0);
  }
  float yMargin() const {
    return k_marginInPixels*ratio() + (verticallyCapped() ? 0 : (std::abs(vectorX())/k_xyRatio - std::abs(vectorY())) / 2.f);
  }
  bool verticallyCapped() const {
    return std::abs(vectorY())*k_xyRatio > std::abs(vectorX());
  }

  float m_vector[2];
};

}

#endif
