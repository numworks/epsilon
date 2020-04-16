#include "chi_squared_distribution.h"
#include "regularized_gamma.h"
#include <cmath>
#include <algorithm>

namespace Probability {

float ChiSquaredDistribution::xMin() const {
  return -k_displayLeftMarginRatio * xMax();
}

float ChiSquaredDistribution::xMax() const {
  assert(m_parameter1 != 0.0f);
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
  if (x < 0.0f) {
    return NAN;
  }
  const float halfk = m_parameter1/2.0f;
  const float halfX = x/2.0f;
  return std::exp(-lgamma(halfk) - halfX + (halfk-1.0f) * std::log(halfX)) / 2.0f;
}

bool ChiSquaredDistribution::authorizedValueAtIndex(float x, int index) const {
  assert(index == 0);
  return x > 0.0f && x == (float)((int)x) && x <= k_maxK;
}

double ChiSquaredDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  if (x < DBL_EPSILON) {
    return 0.0;
  }
  double result = 0.0;
  if (regularizedGamma(m_parameter1/2.0f, x/2.0, k_regularizedGammaPrecision, k_maxRegularizedGammaIterations, &result)) {
    return result;
  }
  return NAN;
}

double ChiSquaredDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  /* We have to compute the values of the interval in which to look for x.
   * We cannot put xMin because xMin is < 0 for display purposes, and negative
   * values are not accepted.
   * The maximum of the interval: we want
   *    cumulativeDistributiveFunctionAtAbscissa(b) > proba
   * => regularizedGamma(halfk, b/2, k_regularizedGammaPrecision) >proba
   * => int(exp(-t)*t^(k/2 - 1), t, 0, b/2)/gamma(k/2) > proba
   * => int(exp(-t)*t^(k/2 - 1), t, 0, b/2) > proba * gamma(k/2)
   *
   * true if: for (k/2 - 1) > 0 which is k > 2
   *            (b/2 - 0) * exp(-(k/2 - 1))*(k/2 - 1)^(k/2 - 1) > proba * gamma(k/2)
   *         => b/2 * exp(-(k/2 - 1) (1 - ln(k/2 - 1))) > proba * gamma(k/2)
   *         => b > 2 * proba * gamma(k/2) / exp(-(k/2 - 1) (1 + ln(k/2 - 1)))
   *          else
   *            2/k * (b/2)^(k/2) > proba * gamma(k/2)
   *         => b^(k/2) > k/2 * 2^(k/2) * proba * gamma(k/2)
   *         => exp(k/2 * ln(b)) > k/2 * 2^(k/2) * proba * gamma(k/2)
   *         => b > exp(2/k * ln(k/2 * 2^(k/2) * proba * gamma(k/2))) */
  if (*probability < DBL_EPSILON) {
    return 0.0;
  }
  const double k = m_parameter1;
  const double ceilKOver2 = std::ceil(k/2.0f);
  const double kOver2Minus1 = k/2.0f - 1.0f;
  double xmax = m_parameter1 > 2.0f ?
    2.0 * *probability * std::exp(std::lgamma(ceilKOver2)) / (exp(-kOver2Minus1) * std::pow(kOver2Minus1, kOver2Minus1)) :
    30.0; // Ad hoc value
  xmax = std::isnan(xmax) ? 1000000000.0 : xmax;
  return cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(probability, FLT_EPSILON, std::max<double>(xMax(), xmax));
}

}
