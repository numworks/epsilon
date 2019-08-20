#include "chi_squared_law.h"
#include "regularized_gamma.h"
#include <cmath>

namespace Probability {

static inline double maxDouble(double x, double y) { return x > y ? x : y; }

float ChiSquaredLaw::xMin() const {
  return -k_displayLeftMarginRatio * xMax();
}

float ChiSquaredLaw::xMax() const {
  assert(m_parameter1 != 0);
  return (m_parameter1 + 5.0f * std::sqrt(m_parameter1)) * (1.0f + k_displayRightMarginRatio);
}

float ChiSquaredLaw::yMax() const {
  float result;
  if (m_parameter1/2.0 <= 1 + FLT_EPSILON) {
    result = 0.5f;
  } else {
    result = evaluateAtAbscissa(m_parameter1 - 1.0) * 1.2;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

float ChiSquaredLaw::evaluateAtAbscissa(float x) const {
  if (x < 0) {
    return NAN;
  }
  const float halfk = m_parameter1/2;
  const float halfX = x/2;
  return coefficient() * std::pow(halfX, halfk-1) * std::exp(-halfX);
}

bool ChiSquaredLaw::authorizedValueAtIndex(float x, int index) const {
  assert(index == 0);
  return x > 0 && x == (int)x;
}

double ChiSquaredLaw::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  if (x < 0) {
    return 0;
  }
  const float halfk = m_parameter1/2;
  double result = 0;
  if (regularizedGamma(halfk, x/2, k_regularizedGammaPrecision, k_maxRegularizedGammaIterations, &result)) {
    return result;
  }
  return NAN;
}

double ChiSquaredLaw::cumulativeDistributiveInverseForProbability(double * probability) {
  /* We have to compute the values of the interval in chich to look for x.
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

  const double k = m_parameter1;
  const double ceilKOver2 = std::ceil(k/2);
  const double kOver2Minus1 = k/2.0 - 1.0;
  double xmax = m_parameter1 > 2.0 ?
    2 * *probability * std::exp(std::lgamma(ceilKOver2)) / (exp(-kOver2Minus1) * std::pow(kOver2Minus1, kOver2Minus1)) :
    30.0; // Ad hoc value
  return cumulativeDistributiveInverseForProbabilityUsingBrentRoots(probability, DBL_EPSILON, maxDouble(xMax(), xmax));
}

float ChiSquaredLaw::coefficient() const {
  const float halfk = m_parameter1/2;
  return 1 / (2 * std::exp(std::lgamma(halfk)));
}

}
