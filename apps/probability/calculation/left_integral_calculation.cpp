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

Calculation::Type LeftIntegralCalculation::type() {
  return Type::LeftIntegral;
}

int LeftIntegralCalculation::numberOfParameters() {
  return 2;
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

double LeftIntegralCalculation::upperBound() {
  return m_upperBound;
}

void LeftIntegralCalculation::compute(int indexKnownElement) {
  if (m_law == nullptr) {
    return;
  }
  if (indexKnownElement == 0) {
    m_result = m_law->cumulativeDistributiveFunctionAtAbscissa(m_upperBound);
  } else {
    m_upperBound = m_law->cumulativeDistributiveInverseForProbability(&m_result);
  }
}

}
