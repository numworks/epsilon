#include "left_integral_calculation.h"
#include <assert.h>
#include <ion.h>
#include <math.h>

namespace Probability {

LeftIntegralCalculation::LeftIntegralCalculation() :
  Calculation(),
  m_upperBound(0.0f),
  m_result(0.0f)
{
  compute(0);
}

Calculation::Type LeftIntegralCalculation::type() {
  return Type::LeftIntegral;
}

int LeftIntegralCalculation::numberOfParameters() {
  return 2;
}

const char * LeftIntegralCalculation::legendForParameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    constexpr static char comparison[] = {'P', '(', 'X', Ion::Charset::LessEqual, 0};
    return comparison;
  }
  return ")=";
}

void LeftIntegralCalculation::setParameterAtIndex(float f, int index) {
  assert(index >= 0 && index < 2);
  float rf = roundf(f/k_precision)*k_precision;
  if (index == 0) {
    m_upperBound = rf;
  }
  if (index == 1) {
    m_result = rf;
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
    /* Results in probability application are rounder to 3 decimals */
    m_result = roundf(m_result/k_precision)*k_precision;
  } else {
    m_upperBound = m_law->cumulativeDistributiveInverseForProbability(&m_result);
    m_upperBound = roundf(m_upperBound/k_precision)*k_precision;
  }
}

}
