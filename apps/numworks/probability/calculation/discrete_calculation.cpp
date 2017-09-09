#include "discrete_calculation.h"
#include <assert.h>
#include <ion.h>
#include <cmath>

namespace Probability {

DiscreteCalculation::DiscreteCalculation() :
  Calculation(),
  m_abscissa(0.0),
  m_result(0.0)
{
  compute(0);
}

Calculation::Type DiscreteCalculation::type() {
  return Type::Discrete;
}

int DiscreteCalculation::numberOfParameters() {
  return 2;
}

int DiscreteCalculation::numberOfEditableParameters() {
  return 1;
}

const I18n::Message *DiscreteCalculation::legendForParameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &I18n::Common::DiscreteLegend;
  }
  return &I18n::Common::LeftIntegralSecondLegend;
}

void DiscreteCalculation::setParameterAtIndex(double f, int index) {
  assert(index == 0);
  double rf = std::round(f);
  m_abscissa = rf;
  compute(index);
}


double DiscreteCalculation::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_abscissa;
  }
  return m_result;
}

double DiscreteCalculation::lowerBound() {
  return m_abscissa;
}

double DiscreteCalculation::upperBound() {
  return m_abscissa;
}

void DiscreteCalculation::compute(int indexKnownElement) {
  if (m_law == nullptr) {
    return;
  }
  m_result = m_law->evaluateAtDiscreteAbscissa(m_abscissa);
  /* Results in probability application are rounder to 3 decimals */
  m_result = std::round(m_result/k_precision)*k_precision;
}

}
