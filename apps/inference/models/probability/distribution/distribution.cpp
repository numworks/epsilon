#include "distribution.h"
#include "binomial_distribution.h"
#include "chi_squared_distribution.h"
#include "exponential_distribution.h"
#include "fisher_distribution.h"
#include "geometric_distribution.h"
#include "normal_distribution.h"
#include "poisson_distribution.h"
#include "student_distribution.h"
#include "uniform_distribution.h"
#include <poincare/solver.h>
#include <cmath>
#include <float.h>

namespace Inference {

bool Distribution::Initialize(Distribution * distribution, Poincare::Distribution::Type type) {
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
  return m_distribution->EvaluateAtAbscissa(x, constParametersArray());
}

void Distribution::setParameterAtIndex(double f, int index) {
  Inference::setParameterAtIndex(f, index);
  computeCurveViewRange();
}

double Distribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return m_distribution->CumulativeDistributiveFunctionAtAbscissa(x, constParametersArray());
}

double Distribution::rightIntegralFromAbscissa(double x) const {
  if (isContinuous()) {
    return 1.0 - cumulativeDistributiveFunctionAtAbscissa(x);
  }
  return 1.0 - cumulativeDistributiveFunctionAtAbscissa(x-1.0);
}

double Distribution::finiteIntegralBetweenAbscissas(double a, double b) const {
  if (b < a) {
    return 0.0;
  }
  if (a == b) {
    return evaluateAtDiscreteAbscissa(a);
  }
  if (isContinuous()) {
    return cumulativeDistributiveFunctionAtAbscissa(b) - cumulativeDistributiveFunctionAtAbscissa(a);
  }
  int start = std::round(a);
  int end = std::round(b);
  double result = 0.0;
  for (int k = start; k <=end; k++) {
    result += evaluateAtDiscreteAbscissa(k);
    /* Avoid too long loop */
    if (k-start > k_maxNumberOfOperations) {
      break;
    }
    if (result >= k_maxProbability) {
      result = 1.0;
      break;
    }
  }
  return result;
}

double Distribution::cumulativeDistributiveInverseForProbability(double * p) {
    return m_distribution->CumulativeDistributiveInverseForProbability(*p, constParametersArray());
}

double Distribution::rightIntegralInverseForProbability(double * probability) {
  if (isContinuous()) {
    double f = 1.0 - *probability;
    return cumulativeDistributiveInverseForProbability(&f);
  }
  if (*probability >= 1.0) {
    return 0.0;
  }
  if (*probability <= 0.0) {
    return INFINITY;
  }
  double p = 0.0;
  int k = 0;
  double delta = 0.0;
  do {
    delta = std::fabs(1.0-*probability-p);
    p += evaluateAtDiscreteAbscissa(k++);
    if (p >= k_maxProbability && std::fabs(1.0-*probability-p) <= delta) {
      *probability = 0.0;
      return k;
    }
  } while (std::fabs(1.0-*probability-p) <= delta && k < k_maxNumberOfOperations);
  if (k == k_maxNumberOfOperations) {
    *probability = 1.0;
    return INFINITY;
  }
  *probability = 1.0 - (p - evaluateAtDiscreteAbscissa(k-1));
  if (std::isnan(*probability)) {
    return NAN;
  }
  return k-1.0;
}

double Distribution::evaluateAtDiscreteAbscissa(int k) const {
  if (isContinuous()) {
    return 0.0;
  }
  return m_distribution->EvaluateAtAbscissa(static_cast<double>(k), constParametersArray());
}

}
