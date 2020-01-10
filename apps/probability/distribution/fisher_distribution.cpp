#include "fisher_distribution.h"
#include <poincare/normal_distribution.h>
#include <cmath>
#include <float.h>

namespace Probability {

float FisherDistribution::xMin() const {
  return -k_displayLeftMarginRatio * xMax();
}

float FisherDistribution::xMax() const {
  return 10.0f; //TODO LEA
}

float FisherDistribution::yMax() const {
  float maxAbscissa = m_parameter1;
  float result = evaluateAtAbscissa(maxAbscissa);
  if (std::isnan(result) || result <= 0.0f) {
    result = 1.0f;
  }
  return 1.0f; //TODO LEA
}

I18n::Message FisherDistribution::parameterNameAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::D1;
  }
  assert(index == 1);
  return I18n::Message::D2;
}

I18n::Message FisherDistribution::parameterDefinitionAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::D1FisherDefinition;
  }
  assert(index == 1);
  return I18n::Message::D2FisherDefinition;
}

float FisherDistribution::evaluateAtAbscissa(float x) const {
  return 1.0; //TODO LEA Poincare::FisherDistribution::EvaluateAtAbscissa(x, m_parameter1, m_parameter2);
}

bool FisherDistribution::authorizedValueAtIndex(float x, int index) const {
  assert(index == 0 || index == 1);
  return x > FLT_MIN; // TODO LEA Add max limit? See NormalDistribution
}

void FisherDistribution::setParameterAtIndex(float f, int index) {
  TwoParameterDistribution::setParameterAtIndex(f, index);
  // TODO LEA Adjust parameter if max definition ? See NormalDistribution
}

double FisherDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return 1.0; //TODO LEA
}

double FisherDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  return 1.0; //TODO LEA
}

}
