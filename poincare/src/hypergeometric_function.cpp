#include <assert.h>
#include <math.h>
#include <poincare/hypergeometric_function.h>

#include <cmath>

namespace Poincare {

bool HypergeometricFunction(double a, double b, double c, double z,
                            double epsilon, int maxNumberOfIterations,
                            double* result) {
  // TODO Put interruption instead of maxNumberOfIterations
  assert(!std::isnan(a) && !std::isnan(b) && !std::isnan(c) && !std::isnan(z));
  if (z == 0.0) {
    *result = 0.0;
    return true;
  }
  /* For x < s + 1: Compute using the infinite series representation
   * hypergeometricFunction(a,b,c,z) = sum((a)n * (b)n * z^n / ((c)n * n!...)
   * With (a)n = a(a+1)..(a+n-1)*/
  if (std::fabs(z) < 1.0) {
    assert(c > 0.0);
    return InfiniteSeriesEvaluation(
        1.0,
        [](double previousTerm, double index, double a, double b, double c,
           double z) {
          return previousTerm * (a + index - 1) * (b + index - 1) * z /
                 ((c + index - 1) * index);
        },
        epsilon, maxNumberOfIterations, result, a, b, c, z);
  }
  return NAN;  // TODO
}

}  // namespace Poincare
