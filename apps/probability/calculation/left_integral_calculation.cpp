#include "left_integral_calculation.h"
#include <assert.h>
#include <math.h>

namespace Probability {

LeftIntegralCalculation::LeftIntegralCalculation() :
  Calculation(),
  m_upperBound(0.0f),
  m_result(0.0f)
{
  compute(0);
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
    m_upperBound = f;
  }
  if (index == 1) {
    m_result = f;
  }
  compute(index);
}


float LeftIntegralCalculation::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_upperBound;
  }
  return m_result;
}

float LeftIntegralCalculation::upperBound() {
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
