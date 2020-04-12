#include "fisher_distribution.h"
#include <poincare/beta_function.h>
#include <poincare/regularized_incomplete_beta_function.h>
#include <cmath>
#include <float.h>
#include <algorithm>

namespace Probability {

float FisherDistribution::xMin() const {
  return -k_displayLeftMarginRatio * xMax();
}

float FisherDistribution::xMax() const {
  return 5.0f; // The mode is always < 1
}

float FisherDistribution::yMax() const {
  const float m = mode();
  float max = std::isnan(m) ? k_defaultMax : evaluateAtAbscissa(m);
  max = std::isnan(max) ? k_defaultMax : max;
  return max * (1.0f + k_displayTopMarginRatio);
}

I18n::Message FisherDistribution::parameterNameAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::D1;
  }
  assert(index == 1);
  return I18n::Message::D2;
}

I18n::Message FisherDistribution::parameterDefinitionAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::D1FisherDefinition;
  }
  assert(index == 1);
  return I18n::Message::D2FisherDefinition;
}

float FisherDistribution::evaluateAtAbscissa(float x) const {
  if (x < 0.0f) {
    return NAN;
  }
  const float d1 = m_parameter1;
  const float d2 = m_parameter2;
  const float f = d1*x/(d1*x+d2);
  const float numerator = std::pow(f, d1/2.0f) * std::pow(1.0f - f, d2/2.0f);
  const float denominator = x * Poincare::BetaFunction(d1/2.0f, d2/2.0f);
  return numerator / denominator;
}

bool FisherDistribution::authorizedValueAtIndex(float x, int index) const {
  assert(index == 0 || index == 1);
  return x > FLT_MIN && x <= k_maxParameter;
}

void FisherDistribution::setParameterAtIndex(float f, int index) {
  TwoParameterDistribution::setParameterAtIndex(f, index);
}

double FisherDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  const double d1 = m_parameter1;
  const double d2 = m_parameter2;
  return Poincare::RegularizedIncompleteBetaFunction(d1/2.0, d2/2.0, d1*x/(d1*x+d2));
}

double FisherDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  /* We have to compute the values of the interval in which to look for x.
   * We cannot put xMin because xMin is < 0 for display purposes, and negative
   * values are not accepted.
   * The maximum of the interval: we want */
  if (*probability < DBL_EPSILON) {
    return 0.0;
  }
  return cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(probability, DBL_EPSILON, std::max<double>(xMax(), 100.0));  // Ad-hoc value;
}

float FisherDistribution::mode() const {
  const float d1 = m_parameter1;
  if (d1 > 2.0f) {
    const float d2 = m_parameter2;
    return (d1 - 2.0f)/d1 * d2/(d2 + 2.0f);
  }
  return NAN;
}

}
