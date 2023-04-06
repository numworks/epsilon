#include "chi_squared_distribution.h"

#include <poincare/chi2_distribution.h>

#include <cmath>

namespace Distributions {

float ChiSquaredDistribution::evaluateAtAbscissa(float x) const {
  return Poincare::Chi2Distribution::EvaluateAtAbscissa<float>(x, m_parameter);
}

bool ChiSquaredDistribution::authorizedParameterAtIndex(double x,
                                                        int index) const {
  assert(index == 0);
  if (!OneParameterDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  return x > 0.0 && x == (double)((int)x) && x <= k_maxK;
}

double ChiSquaredDistribution::cumulativeDistributiveFunctionAtAbscissa(
    double x) const {
  return Poincare::Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa(
      x, m_parameter);
}

double ChiSquaredDistribution::cumulativeDistributiveInverseForProbability(
    double p) const {
  return Poincare::Chi2Distribution::
      CumulativeDistributiveInverseForProbability(p, m_parameter);
}

float ChiSquaredDistribution::privateComputeXMax() const {
  assert(m_parameter != 0.0);
  return (m_parameter + 5.0f * std::sqrt(m_parameter)) *
         (1.0f + k_displayRightMarginRatio);
}

float ChiSquaredDistribution::computeYMax() const {
  float result;
  if (m_parameter / 2.0f <= 1.0f + FLT_EPSILON) {
    result = 0.5f;
  } else {
    result = evaluateAtAbscissa(m_parameter - 1.0f) * 1.2f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

}  // namespace Distributions
