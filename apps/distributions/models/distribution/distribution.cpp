#include "distribution.h"

#include <float.h>
#include <poincare/solver.h>

#include <cmath>

#include "binomial_distribution.h"
#include "chi_squared_distribution.h"
#include "exponential_distribution.h"
#include "fisher_distribution.h"
#include "geometric_distribution.h"
#include "hypergeometric_distribution.h"
#include "normal_distribution.h"
#include "poisson_distribution.h"
#include "student_distribution.h"
#include "uniform_distribution.h"

namespace Distributions {

bool Distribution::Initialize(Distribution* distribution,
                              Poincare::Distribution::Type type) {
  if (distribution->type() == type) {
    return false;
  }
  distribution->~Distribution();
  switch (type) {
    case Poincare::Distribution::Type::Binomial:
      new (distribution) BinomialDistribution();
      break;
    case Poincare::Distribution::Type::Uniform:
      new (distribution) UniformDistribution();
      break;
    case Poincare::Distribution::Type::Exponential:
      new (distribution) ExponentialDistribution();
      break;
    case Poincare::Distribution::Type::Normal:
      new (distribution) NormalDistribution();
      break;
    case Poincare::Distribution::Type::ChiSquared:
      new (distribution) ChiSquaredDistribution();
      break;
    case Poincare::Distribution::Type::Student:
      new (distribution) StudentDistribution();
      break;
    case Poincare::Distribution::Type::Geometric:
      new (distribution) GeometricDistribution();
      break;
    case Poincare::Distribution::Type::Hypergeometric:
      new (distribution) HypergeometricDistribution();
      break;
    case Poincare::Distribution::Type::Poisson:
      new (distribution) PoissonDistribution();
      break;
    case Poincare::Distribution::Type::Fisher:
      new (distribution) FisherDistribution();
      break;
    default:
      assert(false);
  }
  return true;
}

float Distribution::evaluateAtAbscissa(float x) const {
  return m_distribution->evaluateAtAbscissa(x, constParametersArray());
}

bool Distribution::authorizedParameterAtIndex(double x, int index) const {
  if (canHaveUninitializedParameter() &&
      (m_indexOfUninitializedParameter == index ||
       m_indexOfUninitializedParameter == k_allParametersAreInitialized) &&
      std::isnan(x)) {
    // Accept only one uninitialized parameter
    return true;
  }
  return Inference::authorizedParameterAtIndex(x, index);
}

void Distribution::setParameterAtIndex(double f, int index) {
  setParameterAtIndexWithoutComputingCurveViewRange(f, index);
  computeCurveViewRange();
}

void Distribution::setParameterAtIndexWithoutComputingCurveViewRange(
    double x, int index) {
  if (canHaveUninitializedParameter() && std::isnan(x)) {
    assert(m_indexOfUninitializedParameter == index ||
           m_indexOfUninitializedParameter == k_allParametersAreInitialized);
    x = defaultParameterAtIndex(index);
    m_indexOfUninitializedParameter = index;
  } else if (m_indexOfUninitializedParameter == index) {
    m_indexOfUninitializedParameter = k_allParametersAreInitialized;
  }
  assert(canHaveUninitializedParameter() ||
         m_indexOfUninitializedParameter == k_allParametersAreInitialized);
  Inference::setParameterAtIndex(x, index);
}

double Distribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return m_distribution->cumulativeDistributiveFunctionAtAbscissa(
      x, constParametersArray());
}

double Distribution::rightIntegralFromAbscissa(double x) const {
  if (isContinuous()) {
    return 1.0 - cumulativeDistributiveFunctionAtAbscissa(x);
  }
  return 1.0 - cumulativeDistributiveFunctionAtAbscissa(x - 1.0);
}

double Distribution::finiteIntegralBetweenAbscissas(double a, double b) const {
  if (b < a) {
    return 0.0;
  }
  if (a == b) {
    return evaluateAtDiscreteAbscissa(a);
  }
  if (isContinuous()) {
    return cumulativeDistributiveFunctionAtAbscissa(b) -
           cumulativeDistributiveFunctionAtAbscissa(a);
  }
  int start = std::round(a);
  int end = std::round(b);
  double result = 0.0;
  for (int k = start; k <= end; k++) {
    result += evaluateAtDiscreteAbscissa(k);
    /* Avoid too long loop */
    if (k - start > k_maxNumberOfOperations) {
      break;
    }
    if (result >= k_maxProbability) {
      result = 1.0;
      break;
    }
  }
  return result;
}

double Distribution::cumulativeDistributiveInverseForProbability(
    double p) const {
  return m_distribution->cumulativeDistributiveInverseForProbability(
      p, constParametersArray());
}

double Distribution::rightIntegralInverseForProbability(
    double probability) const {
  if (isContinuous()) {
    double f = 1.0 - probability;
    return cumulativeDistributiveInverseForProbability(f);
  }
  if (probability >= 1.0) {
    return 0.0;
  }
  if (probability <= 0.0) {
    return INFINITY;
  }
  double p = 0.0;
  int k = 0;
  double delta = 0.0;
  do {
    delta = std::fabs(1.0 - probability - p);
    p += evaluateAtDiscreteAbscissa(k++);
    if (p >= k_maxProbability && std::fabs(1.0 - probability - p) <= delta) {
      return k;
    }
  } while (std::fabs(1.0 - probability - p) <= delta &&
           k < k_maxNumberOfOperations);
  if (k == k_maxNumberOfOperations) {
    return INFINITY;
  }
  if (std::isnan(1.0 - (p - evaluateAtDiscreteAbscissa(k - 1)))) {
    return NAN;
  }
  return k - 1.0;
}

double Distribution::evaluateAtDiscreteAbscissa(int k) const {
  if (isContinuous()) {
    return 0.0;
  }
  return m_distribution->evaluateAtAbscissa(static_cast<double>(k),
                                            constParametersArray());
}

void Distribution::computeUnknownParameterForProbabilityAndBound(
    double probability, double bound, bool isUpperBound) {
  assert(m_indexOfUninitializedParameter != k_allParametersAreInitialized &&
         canHaveUninitializedParameter());
  double paramValue = m_distribution->evaluateParameterForProbabilityAndBound(
      m_indexOfUninitializedParameter, parametersArray(), probability, bound,
      isUpperBound);
  Inference::setParameterAtIndex(paramValue, m_indexOfUninitializedParameter);
  if (std::isfinite(paramValue)) {
    computeCurveViewRange();
  }
}

}  // namespace Distributions
