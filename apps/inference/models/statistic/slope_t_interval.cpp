#include "slope_t_interval.h"

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

void SlopeTInterval::privateCompute() {
  double n = doubleCastedNumberOfPairsOfSeries(m_series);
  m_degreesOfFreedom = n - 2.0;
  m_SE = computeStandardError();
  m_estimate = slope(m_series);
}

}  // namespace Inference
