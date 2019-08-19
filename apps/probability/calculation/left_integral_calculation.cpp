#include "left_integral_calculation.h"
#include <assert.h>
#include <ion.h>
#include <cmath>

namespace Probability {

LeftIntegralCalculation::LeftIntegralCalculation() :
  Calculation(),
  m_upperBound(0.0),
  m_result(0.0)
{
  compute(0);
}

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
  if (m_law == nullptr) {
    return;
  }
  if (indexKnownElement == 0) {
    m_result = m_law->cumulativeDistributiveFunctionAtAbscissa(m_upperBound);
  } else {
    double currentResult = m_law->cumulativeDistributiveFunctionAtAbscissa(m_upperBound);
    if (std::fabs(currentResult - m_result) < std::pow(10.0, - Constant::LargeNumberOfSignificantDigits)) {
      return;
    }
    m_upperBound = m_law->cumulativeDistributiveInverseForProbability(&m_result);
  }
}

}
