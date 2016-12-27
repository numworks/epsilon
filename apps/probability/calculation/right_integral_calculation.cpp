#include "right_integral_calculation.h"
#include <assert.h>
#include <math.h>

namespace Probability {

RightIntegralCalculation::RightIntegralCalculation() :
  Calculation(),
  m_upperBound(0.0f),
  m_result(0.0f)
{
  compute(0);
}

int RightIntegralCalculation::numberOfParameters() {
  return 2;
}

const char * RightIntegralCalculation::legendForParameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return "P( ";
  }
  return " <= X ) = ";
}

void RightIntegralCalculation::setParameterAtIndex(float f, int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    m_upperBound = f;
  }
  if (index == 1) {
    m_result = f;
  }
  compute(index);
}


float RightIntegralCalculation::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_upperBound;
  }
  return m_result;
}

float RightIntegralCalculation::upperBound() {
  return m_upperBound;
}

void RightIntegralCalculation::compute(int indexKnownElement) {
  if (m_law == nullptr) {
    return;
  }
  if (indexKnownElement == 0) {
    m_result = m_law->rightIntegralFromAbscissa(m_upperBound);
  } else {
    m_upperBound = m_law->rightIntegralInverseForProbability(&m_result);
  }
}

}
