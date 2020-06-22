#include "normal_distribution.h"
#include <poincare/normal_distribution.h>
#include <cmath>
#include <float.h>

namespace Probability {

float NormalDistribution::yMax() const {
  float maxAbscissa = m_parameter1;
  float result = evaluateAtAbscissa(maxAbscissa);
  if (std::isnan(result) || result <= 0.0f) {
    result = 1.0f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

I18n::Message NormalDistribution::parameterNameAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::Mu;
  }
  assert(index == 1);
  return I18n::Message::Sigma;
}

I18n::Message NormalDistribution::parameterDefinitionAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::MeanDefinition;
  }
  assert(index == 1);
  return I18n::Message::DeviationDefinition;
}

float NormalDistribution::evaluateAtAbscissa(float x) const {
  return Poincare::NormalDistribution::EvaluateAtAbscissa<float>(x, m_parameter1, m_parameter2);
}

bool NormalDistribution::authorizedValueAtIndex(float x, int index) const {
  if (index == 0) {
    return true;
  }
  if (x <= FLT_MIN || std::fabs(m_parameter1/x) > k_maxRatioMuSigma) {
    return false;
  }
  return true;
}

void NormalDistribution::setParameterAtIndex(float f, int index) {
  TwoParameterDistribution::setParameterAtIndex(f, index);
  if (index == 0 && std::fabs(m_parameter1/m_parameter2) > k_maxRatioMuSigma) {
    m_parameter2 = m_parameter1/k_maxRatioMuSigma;
  }
}

double NormalDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return Poincare::NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(x, m_parameter1, m_parameter2);
}

double NormalDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  return Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<double>(*probability, m_parameter1, m_parameter2);
}

float NormalDistribution::xExtremum(bool min) const {
  int coefficient = (min ? -1 : 1);
  if (m_parameter2 == 0.0f) {
    return m_parameter1 + coefficient * 1.0f;
  }
  return m_parameter1 + coefficient * 5.0f * std::fabs(m_parameter2);
}

}
