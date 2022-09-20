#ifndef CALCULATION_ADDITIONAL_OUTPUTS_FUNCTION_MODEL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_FUNCTION_MODEL_H

#include "../../shared/memoized_curve_view_range.h"
#include "illustrated_expressions_list_controller.h"
#include <poincare/expression.h>

namespace Calculation {

class FunctionModel : public Shared::MemoizedCurveViewRange {
public:
  void setParameters(Poincare::Expression function, float abscissa);
  Poincare::Expression function() const { return m_function; }
  float abscissa() const { return m_abscissa; }
  float ordinate() const { return m_ordinate; }
  /* Since additional results are opened in a UserCircuitBreakerCheckpoint, we
   * need to clean all calculated expressions when closing it. */
  void tidy() { m_function = Poincare::Expression(); }
private:
  // Margin used to separate the drawings from the curve view's border
  constexpr static int k_marginInPixels = 12;
  constexpr static int k_width = Ion::Display::Width - Escher::Metric::PopUpRightMargin - Escher::Metric::PopUpLeftMargin - 2 * k_marginInPixels;
  constexpr static int k_height = IllustratedExpressionsListController::k_illustrationHeight - 2 * k_marginInPixels;
  constexpr static float k_xyRatio = static_cast<float>(k_width) / static_cast<float>(k_height);
  void recomputeViewRange();
  Poincare::Expression m_function;
  float m_abscissa;
  float m_ordinate;
};

}

#endif
