#include "geometric_distribution.h"
#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Probability {

float GeometricDistribution::xMin() const {
  return -k_displayLeftMarginRatio * xMax();
}

float GeometricDistribution::xMax() const {
  assert(m_parameter1 != 0.0f);
  return 5/m_parameter1 * (1.0f + k_displayRightMarginRatio);
}

float GeometricDistribution::yMax() const {
  int maxAbscissa = 0;
  float result = evaluateAtAbscissa(maxAbscissa);
  return result * (1.0f + k_displayTopMarginRatio);
}

bool GeometricDistribution::authorizedValueAtIndex(float x, int index) const {
  assert(index == 0);
  if (x <= 0.0f || x > 1.0f) {
    return false;
  }
  return true;
}

template<typename T>
T GeometricDistribution::templatedApproximateAtAbscissa(T x) const {
  if (x < 0) {
    return NAN;
  }
  T p = (T)m_parameter1;
  if (p == (T)1.0) {
    return (T)(x == 0 ? 1.0 : 0.0);
  }
  T lResult = x * std::log(((T)1.0) - p);
  return p*std::exp(lResult);
}

}

template float Probability::GeometricDistribution::templatedApproximateAtAbscissa(float x) const;
template double Probability::GeometricDistribution::templatedApproximateAtAbscissa(double x) const;
