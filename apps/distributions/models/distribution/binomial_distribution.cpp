#include "binomial_distribution.h"

#include <assert.h>
#include <poincare/layout.h>

#include <cmath>

using namespace Shared;

namespace Distributions {

double BinomialDistribution::rightIntegralInverseForProbability(
    double p) const {
  if (m_parameters[0] == 0.0 &&
      (m_parameters[1] == 0.0 || m_parameters[1] == 1.0)) {
    return NAN;
  }
  if (p <= 0.0) {
    return m_parameters[0];
  }
  return Distribution::rightIntegralInverseForProbability(p);
}

float BinomialDistribution::privateComputeXMin() const {
  float min = 0.0f;
  float max = m_parameters[0] > min ? m_parameters[0] : 1.0f;
  return min - k_displayLeftMarginRatio * (max - min);
}

float BinomialDistribution::privateComputeXMax() const {
  float min = 0.0f;
  float max = m_parameters[0] > min ? m_parameters[0] : 1.0f;
  return max + k_displayRightMarginRatio * (max - min);
}

float BinomialDistribution::computeYMax() const {
  int maxAbscissa = m_parameters[1] < 1.0
                        ? (m_parameters[0] + 1) * m_parameters[1]
                        : m_parameters[0];
  float result = evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f || std::isnan(result)) {
    result = 1.0f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

}  // namespace Distributions
