#include "distribution.h"
#include <poincare/solver.h>
#include <cmath>
#include <float.h>

namespace Probability {

double Distribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  if (!isContinuous()) {
    return Poincare::Solver::CumulativeDistributiveFunctionForNDefinedFunction<double>(x,
        [](double k, Poincare::Context * context, const void * auxiliary) {
          const Distribution * distribution = static_cast<const Distribution *>(auxiliary);
          return distribution->evaluateAtDiscreteAbscissa(k);
        },
        nullptr, this);
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
      [](double k, Poincare::Context * context, const void * auxiliary) {
        const Distribution * distribution = static_cast<const Distribution *>(auxiliary);
        return distribution->evaluateAtDiscreteAbscissa(k);
      }, nullptr, this);
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
  assert(isContinuous()); // Discrete distributions override this method
  return 0.0;
}

double Distribution::cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(double * probability, double ax, double bx) {
  assert(ax < bx);
  if (*probability > 1.0 - DBL_EPSILON) {
    return INFINITY;
  }
  if (*probability < DBL_EPSILON) {
    return -INFINITY;
  }
  const void * pack[2] = { this, probability };
  Poincare::Coordinate2D<double> result = Poincare::Solver::IncreasingFunctionRoot(
      ax, bx, DBL_EPSILON,
      [](double x, Poincare::Context * context, const void * auxiliary) {
        const void * const * pack = static_cast<const void * const *>(auxiliary);
        const Distribution * distribution = static_cast<const Distribution *>(pack[0]);
        const double * proba = static_cast<const double *>(pack[1]);
        return distribution->cumulativeDistributiveFunctionAtAbscissa(x) - *proba; // This needs to be an increasing function
      },
      nullptr, pack);
  /* Either no result was found, the precision is ok or the result was outside
   * the given ax bx bounds */
   if (!(std::isnan(result.x2()) || std::fabs(result.x2()) <= FLT_EPSILON || std::fabs(result.x1()- ax) < FLT_EPSILON || std::fabs(result.x1() - bx) < FLT_EPSILON)) {
     /* We would like to put this as an assertion, but sometimes we do get
      * false result: we replace them with inf to make the problem obvious to
      * the student. */
     assert(false);  // TODO this assert is used to hunt a case where that happens. If it doesn't, then we can remove this block of code
     return *probability > 0.5 ? INFINITY : -INFINITY;
   }
   return result.x1();
}

float Distribution::yMin() const {
  return -k_displayBottomMarginRatio * yMax();
}

bool Distribution::authorizedValueAtIndex(double x, int index) const {
  return std::isfinite(static_cast<float>(x));
}

}
