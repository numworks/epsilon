#include "finite_integral_calculation.h"

#include <assert.h>

#include "../distribution/normal_distribution.h"

namespace Distributions {

FiniteIntegralCalculation::FiniteIntegralCalculation(Distribution * distribution) :
  Calculation(distribution),
  m_lowerBound(distribution->defaultComputedValue()),
  m_upperBound(m_lowerBound + 1.0)
{}

I18n::Message FiniteIntegralCalculation::legendForParameterAtIndex(int index) {
  assert(index >= 0 && index < 3);
  if (index == 0) {
    return I18n::Message::FiniteIntegralFirstLegend;
  }
  if (index == 1) {
    return I18n::Message::FiniteIntegralLegend;
  }
  return I18n::Message::LeftIntegralSecondLegend;
}

void FiniteIntegralCalculation::setParameterAtIndex(double f, int index) {
  assert(index >= 0 && index < 3);
  if (index == 0) {
    m_lowerBound = f;
  }
  if (index == 1) {
    m_upperBound = f;
  }
  if (index == 2) {
    m_result = f;
  }
  compute(index);
}


double FiniteIntegralCalculation::parameterAtIndex(int index) {
  assert(index >= 0 && index < 3);
  if (index == 0) {
    return m_lowerBound;
  }
  if (index == 1) {
    return m_upperBound;
  }
  return m_result;
}

void FiniteIntegralCalculation::compute(int indexKnownElement) {
  if (m_distribution == nullptr) {
    return;
  }
  if (indexKnownElement == 2) {
    assert(m_distribution->isContinuous() && m_distribution->isSymmetrical());
    double p = (1.0 + m_result) / 2.0;
    m_upperBound = m_distribution->cumulativeDistributiveInverseForProbability(p);
    m_lowerBound = 2.0 * (m_distribution->meanAbscissa()) - m_upperBound;
  }
  m_result = m_distribution->finiteIntegralBetweenAbscissas(m_lowerBound, m_upperBound);
}

}
