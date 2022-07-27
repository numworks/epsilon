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
  float xMin() const override { return std::min(-arcHorizontalSpace(), vectorX()) - xMargin(); }
  float xMax() const override { return std::max(0.f, vectorX()) + xMargin(); }
  float yMin() const override { return std::min(0.f, vectorY()) - yMargin(); }
  float yMax() const override { return std::max(arcVerticalSpace(), vectorY()) + yMargin(); }

  void setVectorX(float f) { m_vector[0] = f; }
  void setVectorY(float f) { m_vector[1] = f; }
  float vectorX() const { return m_vector[0]; }
  float vectorY() const { return m_vector[1]; }
  float ratio(bool precise = true) const { return verticallyCapped(precise) ? height(precise)/k_height : width(precise)/k_width; }
  float arcRadiusInPixels() const { return k_arcRadiusInPixels; }
private:
  constexpr static float k_arcRadiusInPixels = 20;
  constexpr static int k_marginInPixels = 12;
  constexpr static int k_width = Ion::Display::Width - Escher::Metric::PopUpRightMargin - Escher::Metric::PopUpLeftMargin - 2 * k_marginInPixels;
  constexpr static int k_height = IllustratedExpressionsListController::k_illustrationHeight - 2 * k_marginInPixels;
  constexpr static float k_xyRatio = static_cast<float>(k_width) / static_cast<float>(k_height);

  /* When the vector points downward, we need to keep space around the origin
   * for the arc which extends on the other side of the vector. */
  float arcHorizontalSpace() const { return vectorX()>0 && vectorY()<0 && !verticallyCapped(false) ? k_arcRadiusInPixels * ratio(false) : 0.f; }
  float arcVerticalSpace() const { return vectorY()<0 && verticallyCapped(false) ? k_arcRadiusInPixels * ratio(false) : 0.f; }
  /* The arc radius is specified in pixel to be independent of the vector
   * orientation, but this is a problem since its real length depends on the
   * range which may depend on the arc length. To escape the recursion, the
   * margin introduced by the arc is computed with an approximated range. */
  float width(bool precise = true) const { return std::abs(vectorX()) + (precise ? arcHorizontalSpace() : 0.f); }
  float height(bool precise = true) const { return std::abs(vectorY()) + (precise ? arcVerticalSpace() : 0.f); }

  float xMargin() const {
    return k_marginInPixels*ratio() + (verticallyCapped() ? (height()*k_xyRatio - width()) / 2.f : 0.f);
  }
  float yMargin() const {
    return k_marginInPixels*ratio() + (verticallyCapped() ? 0.f : (width()/k_xyRatio - height()) / 2.f);
  }
  bool verticallyCapped(bool precise = true) const {
    return height(precise)*k_xyRatio > width(precise);
  }

  float m_vector[2];
};

}

#endif
