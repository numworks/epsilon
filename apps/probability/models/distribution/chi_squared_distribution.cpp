#include "chi_squared_distribution.h"

#include <cmath>
#include <poincare/chi2_distribution.h>

namespace Probability {

float ChiSquaredDistribution::evaluateAtAbscissa(float x) const {
  return Poincare::Chi2Distribution::EvaluateAtAbscissa<float>(x, m_parameter1);
}

bool ChiSquaredDistribution::authorizedValueAtIndex(double x, int index) const {
  assert(index == 0);
  if (!OneParameterDistribution::authorizedValueAtIndex(x, index)) {
    return false;
  }
  return x > 0.0 && x == (double)((int)x) && x <= k_maxK;
}

double ChiSquaredDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return Poincare::Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa(x, m_parameter1);
}

double ChiSquaredDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  return Poincare::Chi2Distribution::CumulativeDistributiveInverseForProbability(*probability, m_parameter1);
}

float ChiSquaredDistribution::computeXMax() const {
  assert(m_parameter1 != 0.0);
  return (m_parameter1 + 5.0f * std::sqrt(m_parameter1)) * (1.0f + k_displayRightMarginRatio);
}

float ChiSquaredDistribution::computeYMax() const {
  float result;
  if (m_parameter1/2.0f <= 1.0f + FLT_EPSILON) {
    result = 0.5f;
  } else {
    result = evaluateAtAbscissa(m_parameter1 - 1.0f) * 1.2f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

}
