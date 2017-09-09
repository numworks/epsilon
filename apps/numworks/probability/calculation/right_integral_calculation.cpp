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

const I18n::Message *RightIntegralCalculation::legendForParameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &I18n::Common::RightIntegralFirstLegend;
  }
  return &I18n::Common::RightIntegralSecondLegend;
}

void RightIntegralCalculation::setParameterAtIndex(double f, int index) {
  assert(index >= 0 && index < 2);
  double rf = std::round(f/k_precision)*k_precision;
  if (index == 0) {
    m_lowerBound = rf;
  }
  if (index == 1) {
    m_result = rf;
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
    /* Results in probability application are rounder to 3 decimals */
    m_result = std::round(m_result/k_precision)*k_precision;
  } else {
    m_lowerBound = m_law->rightIntegralInverseForProbability(&m_result);
    m_lowerBound = std::round(m_lowerBound/k_precision)*k_precision;
  }
}

}
