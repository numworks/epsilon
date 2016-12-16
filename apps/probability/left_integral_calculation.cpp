#include "left_integral_calculation.h"
#include <assert.h>
#include <math.h>

namespace Probability {

LeftIntegralCalculation::LeftIntegralCalculation() :
  Calculation(),
  m_lowerBound(0.0f),
  m_result(0.0f)
{
  compute(0);
}

Calculation::Type LeftIntegralCalculation::type() const {
  return Type::LeftIntegral;
}

int LeftIntegralCalculation::numberOfParameters() {
  return 2;
}

const char * LeftIntegralCalculation::legendForParameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return "P(X <= ";
  }
  return ") = ";
}

void LeftIntegralCalculation::setParameterAtIndex(float f, int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    m_lowerBound = f;
  }
  if (index == 1) {
    m_result = f;
  }
  compute(index);
}


float LeftIntegralCalculation::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_lowerBound;
  }
  return m_result;
}

void LeftIntegralCalculation::compute(int indexKnownElement) {
  if (m_law == nullptr) {
    return;
  }
  if (indexKnownElement == 0) {
    m_result = m_law->cumulativeDistributiveFunctionAtAbscissa(m_lowerBound);
  } else {
    m_lowerBound = m_law->cumulativeDistributiveInverseForProbability(&m_result);
  }
}

}
