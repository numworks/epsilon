#include "right_integral_calculation.h"
#include <assert.h>
#include <ion.h>
#include <cmath>

namespace Probability {

RightIntegralCalculation::RightIntegralCalculation() :
  Calculation(),
  m_lowerBound(0.0),
  m_result(0.0)
{
  compute(0);
}

Calculation::Type RightIntegralCalculation::type() {
  return Type::RightIntegral;
}

int RightIntegralCalculation::numberOfParameters() {
  return 2;
}

I18n::Message RightIntegralCalculation::legendForParameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::RightIntegralFirstLegend;
  }
  return I18n::Message::RightIntegralSecondLegend;
}

void RightIntegralCalculation::setParameterAtIndex(double f, int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    m_lowerBound = f;
  }
  if (index == 1) {
    m_result = f;
  }
  compute(index);
}

double RightIntegralCalculation::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_lowerBound;
  }
  return m_result;
}

double RightIntegralCalculation::lowerBound() {
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
