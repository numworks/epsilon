#include "fisher_distribution.h"
#include <poincare/beta_function.h>
#include <poincare/layout_helper.h>
#include <poincare/regularized_incomplete_beta_function.h>
#include <cmath>
#include <float.h>
#include <algorithm>

namespace Inference {

float FisherDistribution::evaluateAtAbscissa(float x) const {
  if (x < 0.0f) {
    return NAN;
  }
  const float d1 = m_parameters[0];
  const float d2 = m_parameters[1];
  const float f = d1*x/(d1*x+d2);
  const float numerator = std::pow(f, d1/2.0f) * std::pow(1.0f - f, d2/2.0f);
  const float denominator = x * Poincare::BetaFunction(d1/2.0f, d2/2.0f);
  return numerator / denominator;
}

bool FisherDistribution::authorizedParameterAtIndex(double x, int index) const {
  assert(index == 0 || index == 1);
  if (!TwoParameterDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  return x > DBL_MIN && x <= k_maxParameter;
}

double FisherDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return Poincare::RegularizedIncompleteBetaFunction(m_parameters[0]/2.0, m_parameters[1]/2.0, m_parameters[0]*x/(m_parameters[0]*x+m_parameters[1]));
}

double FisherDistribution::cumulativeDistributiveInverseForProbability(double * p) {
  /* We have to compute the values of the interval in which to look for x.
   * We cannot put computeXMin because computeXMin is < 0 for display purposes, and negative
   * values are not accepted.
   * The maximum of the interval: we want */
  if (*p < DBL_EPSILON) {
    return 0.0;
  }
  return cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(p, DBL_EPSILON, std::max<double>(computeXMax(), 100.0));  // Ad-hoc value;
}

float FisherDistribution::mode() const {
  const float d1 = m_parameters[0];
  if (d1 > 2.0f) {
    const float d2 = m_parameters[1];
    return (d1 - 2.0f)/d1 * d2/(d2 + 2.0f);
  }
  return NAN;
}

ParameterRepresentation FisherDistribution::paramRepresentationAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::D1:
    {
      Poincare::Layout d1 = Poincare::LayoutHelper::String(parameterNameAtIndex(ParamsOrder::D1));
      return ParameterRepresentation{d1, I18n::Message::D1FisherDefinition};
    }
    case ParamsOrder::D2:
    {
      Poincare::Layout d2 = Poincare::LayoutHelper::String(parameterNameAtIndex(ParamsOrder::D2));
      return ParameterRepresentation{d2, I18n::Message::D2FisherDefinition};
    }
    default:
      assert(false);
      return ParameterRepresentation{};
  }
}

float FisherDistribution::computeXMax() const {
  return 5.0f; // The mode is always < 1
}

float FisherDistribution::computeYMax() const {
  const float m = mode();
  float max = std::isnan(m) ? k_defaultMax : evaluateAtAbscissa(m);
  max = std::isnan(max) ? k_defaultMax : max;
  return max * (1.0f + k_displayTopMarginRatio);
}

}
