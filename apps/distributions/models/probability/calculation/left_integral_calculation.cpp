#include "left_integral_calculation.h"
#include "distributions/models/probability/distribution/distribution.h"
#include <poincare/preferences.h>
#include <cmath>
#include <assert.h>

namespace Inference {

LeftIntegralCalculation::LeftIntegralCalculation(Distribution * distribution) :
  Calculation(distribution),
  m_upperBound(distribution->defaultComputedValue())
{}

I18n::Message LeftIntegralCalculation::legendForParameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::LeftIntegralFirstLegend;
  }
  return I18n::Message::LeftIntegralSecondLegend;
}

void LeftIntegralCalculation::setParameterAtIndex(double f, int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    m_upperBound = f;
  }
  if (index == 1) {
    m_result = f;
  }
  compute(index);
}

double LeftIntegralCalculation::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_upperBound;
  }
  return m_result;
}

void LeftIntegralCalculation::compute(int indexKnownElement) {
  if (m_distribution == nullptr) {
    return;
  }
  if (indexKnownElement == 0) {
    m_result = m_distribution->cumulativeDistributiveFunctionAtAbscissa(m_upperBound);
  } else {
    if (!std::isnan(m_upperBound)) {
      double currentResult = m_distribution->cumulativeDistributiveFunctionAtAbscissa(m_upperBound);
      if (std::fabs(currentResult - m_result) < std::pow(10.0, - Poincare::Preferences::VeryLargeNumberOfSignificantDigits)) {
        m_result = currentResult;
        return;
      }
    }
    m_upperBound = m_distribution->cumulativeDistributiveInverseForProbability(m_result);
    m_result = m_distribution->cumulativeDistributiveFunctionAtAbscissa(m_upperBound);
    if (std::isnan(m_upperBound)) {
      m_result = NAN;
    }
  }
}

}
