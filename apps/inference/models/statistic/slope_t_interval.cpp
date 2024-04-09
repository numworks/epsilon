#include "slope_t_interval.h"

#include <poincare/k_tree.h>
#include <poincare/layout.h>

namespace Inference {

void SlopeTInterval::tidy() {
  Interval::tidy();
  DoublePairStore::tidy();
}

Poincare::Layout SlopeTInterval::estimateLayout() const {
  if (m_estimateLayout.isUninitialized()) {
    m_estimateLayout = Poincare::Layout::String(estimateSymbol());
  }
  return m_estimateLayout;
}

Shared::ParameterRepresentation SlopeTInterval::paramRepresentationAtIndex(
    int i) const {
  return Shared::ParameterRepresentation{KRackL(), I18n::Message::Default};
}

void SlopeTInterval::privateCompute() {
  double n = doubleCastedNumberOfPairsOfSeries(m_series);
  m_degreesOfFreedom = n - 2.0;
  m_SE = computeStandardError();
  m_estimate = slope(m_series);
}

}  // namespace Inference
