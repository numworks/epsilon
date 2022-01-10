#include "chi_squared_distribution.h"

#include <cmath>

#include "probability/models/chi2_law.h"

namespace Probability {

float ChiSquaredDistribution::xMin() const {
  return -k_displayLeftMarginRatio * xMax();
}

float ChiSquaredDistribution::xMax() const {
  assert(m_parameter1 != 0.0);
  return (m_parameter1 + 5.0f * std::sqrt(m_parameter1)) * (1.0f + k_displayRightMarginRatio);
}

float ChiSquaredDistribution::yMax() const {
  float result;
  if (m_parameter1/2.0f <= 1.0f + FLT_EPSILON) {
    result = 0.5f;
  } else {
    result = evaluateAtAbscissa(m_parameter1 - 1.0f) * 1.2f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

float ChiSquaredDistribution::evaluateAtAbscissa(float x) const {
  return Chi2Law::EvaluateAtAbscissa<float>(x, m_parameter1);
}

bool ChiSquaredDistribution::authorizedValueAtIndex(double x, int index) const {
  assert(index == 0);
  if (!OneParameterDistribution::authorizedValueAtIndex(x, index)) {
    return false;
  }
  return x > 0.0 && x == (double)((int)x) && x <= k_maxK;
}

double ChiSquaredDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return Chi2Law::CumulativeDistributiveFunctionAtAbscissa(x, m_parameter1);
}

double ChiSquaredDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  return Chi2Law::CumulativeDistributiveInverseForProbability(*probability, m_parameter1);
}

}
