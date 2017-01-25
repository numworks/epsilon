#include "right_integral_calculation.h"
#include <assert.h>
#include <ion.h>
#include <math.h>

namespace Probability {

RightIntegralCalculation::RightIntegralCalculation() :
  Calculation(),
  m_lowerBound(0.0f),
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
    return "P(";
  }
  constexpr static char comparison[] = {Ion::Charset::LessEqual, 'X', ')', '=', 0};
  return comparison;
}

void RightIntegralCalculation::setParameterAtIndex(float f, int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    m_lowerBound = f;
  }
  if (index == 1) {
    m_result = f;
  }
  compute(index);
}


float RightIntegralCalculation::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_lowerBound;
  }
  return m_result;
}

float RightIntegralCalculation::lowerBound() {
  return m_lowerBound;
}

void RightIntegralCalculation::compute(int indexKnownElement) {
  if (m_law == nullptr) {
    return;
  }
  if (indexKnownElement == 0) {
    m_result = m_law->rightIntegralFromAbscissa(m_lowerBound);
  } else {
    m_lowerBound = m_law->rightIntegralInverseForProbability(&m_result);
  }
}

}
