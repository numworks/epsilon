#include "discrete_calculation.h"

#include <assert.h>

#include <cmath>

#include "../distribution/distribution.h"

namespace Distributions {

DiscreteCalculation::DiscreteCalculation(Distribution* distribution)
    : Calculation(distribution),
      m_abscissa(distribution->defaultComputedValue()) {}

I18n::Message DiscreteCalculation::legendForParameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::DiscreteLegend;
  }
  return I18n::Message::LeftIntegralSecondLegend;
}

void DiscreteCalculation::setParameterAtIndex(double f, int index) {
  assert(index == 0);
  double rf = std::round(f);
  m_abscissa = rf;
  compute(index);
}

double DiscreteCalculation::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_abscissa;
  }
  return m_result;
}

void DiscreteCalculation::compute(int indexKnownElement) {
  if (m_distribution == nullptr) {
    return;
  }
  m_result = m_distribution->evaluateAtDiscreteAbscissa(m_abscissa);
}

}  // namespace Distributions
