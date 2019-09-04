#include "distribution.h"
#include <poincare/solver.h>
#include <cmath>
#include <float.h>

namespace Probability {

double Distribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  if (!isContinuous()) {
    return Poincare::Solver::CumulativeDistributiveFunctionForNDefinedFunction<double>(x,
        [](double k, Poincare::Context * context, Poincare::Preferences::ComplexFormat complexFormat, Poincare::Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Distribution * distribution = reinterpret_cast<const Distribution *>(context1);
        return distribution->evaluateAtDiscreteAbscissa(k);
      }, nullptr, Poincare::Preferences::ComplexFormat::Real, Poincare::Preferences::AngleUnit::Degree, this);
    // Context, complex format and angle unit are dummy values
  }
  return 0.0;
}

double Distribution::rightIntegralFromAbscissa(double x) const {
  if (isContinuous()) {
    return 1.0 - cumulativeDistributiveFunctionAtAbscissa(x);
  }
  return 1.0 - cumulativeDistributiveFunctionAtAbscissa(x-1.0);
}

double Distribution::finiteIntegralBetweenAbscissas(double a, double b) const {
  if (b <= a) {
    return 0.0;
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

double Distribution::cumulativeDistributiveInverseForProbability(double * probability) {
  if (*probability > 1.0 - DBL_EPSILON) {
    return INFINITY;
  }
  if (isContinuous()) {
    return 0.0;
  }
  if (*probability < DBL_EPSILON) {
    return -1.0;
  }
  return Poincare::Solver::CumulativeDistributiveInverseForNDefinedFunction<double>(probability,
        [](double k, Poincare::Context * context, Poincare::Preferences::ComplexFormat complexFormat, Poincare::Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Distribution * distribution = reinterpret_cast<const Distribution *>(context1);
        return distribution->evaluateAtDiscreteAbscissa(k);
      }, nullptr, Poincare::Preferences::ComplexFormat::Real, Poincare::Preferences::AngleUnit::Degree, this);
    // Context, complex format and angle unit are dummy values
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
  return 0.0;
}

double Distribution::cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(double * probability, double ax, double bx) {
  assert(ax < bx);
  if (*probability >= 1.0) {
    return INFINITY;
  }
  if (*probability <= 0.0) {
    return -INFINITY;
  }
  double valuePrecision = FLT_EPSILON;
  Poincare::Coordinate2D<double> result = Poincare::Solver::IncreasingFunctionRoot(
      ax,
      bx,
      DBL_EPSILON,
      valuePrecision,
      [](double x, Poincare::Context * context, Poincare::Preferences::ComplexFormat complexFormat, Poincare::Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Distribution * distribution = reinterpret_cast<const Distribution *>(context1);
        const double * proba = reinterpret_cast<const double *>(context2);
        return distribution->cumulativeDistributiveFunctionAtAbscissa(x) - *proba; // This needs to be an increasing function
      },
      nullptr,
      Poincare::Preferences::sharedPreferences()->complexFormat(),
      Poincare::Preferences::sharedPreferences()->angleUnit(),
      this,
      probability,
      nullptr);
  assert(std::isnan(result.x2()) || std::fabs(result.x2()) < valuePrecision);
  return result.x1();
}

float Distribution::yMin() const {
  return -k_displayBottomMarginRatio * yMax();
}

}
