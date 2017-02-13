#include "finite_integral_calculation.h"
#include <assert.h>
#include <ion.h>
#include <math.h>

namespace Probability {

FiniteIntegralCalculation::FiniteIntegralCalculation() :
  Calculation(),
  m_lowerBound(0.0f),
  m_upperBound(1.0f),
  m_result(0.0f)
{
  compute(0);
}

int FiniteIntegralCalculation::numberOfParameters() {
  return 3;
}

const char * FiniteIntegralCalculation::legendForParameterAtIndex(int index) {
  assert(index >= 0 && index < 3);
  if (index == 0) {
    return "P(";
  }
  if (index == 1) {
    constexpr static char comparison[] = {Ion::Charset::LessEqual, 'X', Ion::Charset::LessEqual, 0};
    return comparison;
  }
  return ")=";
}

void FiniteIntegralCalculation::setParameterAtIndex(float f, int index) {
  assert(index >= 0 && index < 3);
  float rf = roundf(f/k_precision)*k_precision;
  if (index == 0) {
    m_lowerBound = rf;
  }
  if (index == 1) {
    m_upperBound = rf;
  }
  if (index == 2) {
    m_result = rf;
  }
  compute(index);
}


float FiniteIntegralCalculation::parameterAtIndex(int index) {
  assert(index >= 0 && index < 3);
  if (index == 0) {
    return m_lowerBound;
  }
  if (index == 1) {
    return m_upperBound;
  }
  return m_result;
}

float FiniteIntegralCalculation::lowerBound() {
  return m_lowerBound;
}

float FiniteIntegralCalculation::upperBound() {
  return m_upperBound;
}

void FiniteIntegralCalculation::compute(int indexKnownElement) {
  if (m_law == nullptr) {
    return;
  }
  m_result = m_law->finiteIntegralBetweenAbscissas(m_lowerBound, m_upperBound);
  /* Results in probability application are rounder to 3 decimals */
  m_result = roundf(m_result/k_precision)*k_precision;
}

}
