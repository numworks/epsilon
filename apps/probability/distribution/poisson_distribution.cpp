#include "poisson_distribution.h"
#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Probability {

float PoissonDistribution::xMin() const {
  return -k_displayLeftMarginRatio * xMax();
}

float PoissonDistribution::xMax() const {
  assert(m_parameter1 != 0);
  return (m_parameter1 + 5.0f * std::sqrt(m_parameter1)) * (1.0f + k_displayRightMarginRatio);
}

float PoissonDistribution::yMax() const {
  int maxAbscissa = (int)m_parameter1;
  assert(maxAbscissa >= 0.0f);
  float result = evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f) {
    result = 1.0f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

bool PoissonDistribution::authorizedValueAtIndex(float x, int index) const {
  if (x <= 0.0f || x > 999.0f) {
    return false;
  }
  return true;
}

template<typename T>
T PoissonDistribution::templatedApproximateAtAbscissa(T x) const {
  if (x < 0) {
    return NAN;
  }
  T lResult = -(T)m_parameter1 + std::floor(x) * std::log((T)m_parameter1) - std::lgamma(std::floor(x) + 1);
  return std::exp(lResult);
}

}

template float Probability::PoissonDistribution::templatedApproximateAtAbscissa(float x) const;
template double Probability::PoissonDistribution::templatedApproximateAtAbscissa(double x) const;
