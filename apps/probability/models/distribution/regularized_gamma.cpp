#include "regularized_gamma.h"
#include "distribution.h"
#include "helper.h"
#include <cmath>
#include <float.h>
#include <assert.h>

bool regularizedGamma(double s, double x, double epsilon, int maxNumberOfIterations, double * result) {
  // TODO Put interruption instead of maxNumberOfIterations

  assert(!std::isnan(s) && !std::isnan(x) && s > 0.0 && x >= 0.0);
  if (x == 0.0) {
    *result = 0.0;
    return true;
  }
  if (std::isinf(x)) {
    *result = 1.0;
    return true;
  }
  if (x >= s + 1.0) {
    /* For x >= s + 1: Compute using the continued fraction representation
     * regularizedGamma(s,x) = 1 - gamma(s,x)/gamma(s)
     *                                                              1
     *                    = 1 - e^-x * x^s / gamma(s) * -----------------------
     *                                                              1(1-s)
     *                                                  1+x-s- ----------------
     *                                                                  2(2-s)
     *                                                         3+x-s- ---------
     *                                                                 x+5-s-...
     *
     *                                                              1
     *                    = 1 - e^-x * x^s / gamma(s) * -----------------------
     *                                                              a(1)
     *                                                  b(0)- ----------------
     *                                                                  a(2)
     *                                                         b(1)- ---------
     *                                                                 b(3)-...
     *
     * With a(n) = n(n-s)
     * With b(n) = (2*n+1) + x - s
     *
     * We have the following formulae:
     * A(n)=b(n)A(n-1)+a(n)A(n-2)  A(-1) = 1  A(0) = b(0)
     * B(n)=b(n)B(n-1)+a(n)B(n-2)  B(-1) = 0  B(0) = 1
     * The consecutive convergents of the continued fraction are : A(n)/B(n)
     * => This method does not work well.
     *
     * Other method : Lentz's method with Thompson and Barnett's modification */

    double continuedFractionValue = 0.0;
    if (!Helper::ContinuedFractionEvaluation(
          [](double index, double s, double x) { return index * (s - index); },
          [](double index, double s, double x) { return (2.0 * index + 1.0) + x - s; },
          maxNumberOfIterations,
          &continuedFractionValue,
          s,
          x))
    {
      return false;
    }
    *result = 1.0 - std::exp(-x + s*std::log(x) - std::lgamma(s)) * ( 1.0 / continuedFractionValue);
    return true;
  }

  /* For x < s + 1: Compute using the infinite series representation
   * incompleteGamma(s,x) = 1/gamma(a) * e^-x * x^s * sum(x^n/((s+n)(s+n-1)...) */
  double infiniteSeriesValue = 0.0;
  if (!Helper::InfiniteSeriesEvaluation(
        1.0/s,
        [](double previousTerm, double index, double s, double x, double d1, double d2) { return previousTerm * x / (s + index); },
        epsilon,
        maxNumberOfIterations,
        &infiniteSeriesValue,
        s,
        x))
  {
    return false;
  }
  *result = std::isinf(infiniteSeriesValue) ? 1.0 : std::exp(-x + s*std::log(x) -  std::lgamma(s)) * infiniteSeriesValue;
  return true;
}
