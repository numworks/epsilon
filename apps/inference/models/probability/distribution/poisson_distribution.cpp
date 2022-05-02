#include "poisson_distribution.h"
#include <poincare/poisson_distribution.h>
#include <assert.h>
#include <cmath>

namespace Inference {

bool PoissonDistribution::authorizedParameterAtIndex(double x, int index) const {
  if (!OneParameterDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  if (x <= 0.0 || x > 999.0) {
    return false;
  }
  return true;
}

template<typename T>
T PoissonDistribution::templatedApproximateAtAbscissa(T x) const {
  return Poincare::PoissonDistribution::EvaluateAtAbscissa<T>(x, static_cast<T>(m_parameter));
}

float PoissonDistribution::computeXMax() const {
  assert(m_parameter != 0);
  return (m_parameter + 5.0f * std::sqrt(m_parameter)) * (1.0f + k_displayRightMarginRatio);
}

float PoissonDistribution::computeYMax() const {
  int maxAbscissa = (int)m_parameter;
  assert(maxAbscissa >= 0.0f);
  float result = evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f) {
    result = 1.0f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

template float PoissonDistribution::templatedApproximateAtAbscissa(float x) const;
template double PoissonDistribution::templatedApproximateAtAbscissa(double x) const;

}
