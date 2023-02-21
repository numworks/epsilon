#include <assert.h>
#include <math.h>
#include <poincare/regularized_gamma_function.h>

#include <cmath>

namespace Poincare {

void increaseIfTooSmall(double* d) {
  assert(!std::isinf(*d) && !std::isnan(*d));
  const double small = 1e-50;
  if (std::fabs(*d) < small) {
    *d = small;
  }
}

// TODO: use in regularized beta function
typedef double (*ValueForIndex)(double index, double s, double x);
static bool ContinuedFractionEvaluation(ValueForIndex a, ValueForIndex b,
                                        int maxNumberOfIterations,
                                        double* result, double context1,
                                        double context2) {
  // Lentz's method with Thompson and Barnett's modification
  double hnminus1 = b(0, context1, context2);
  increaseIfTooSmall(&hnminus1);
  double cnminus1 = hnminus1;
  double dnminus1 = 0.0;
  double hn = hnminus1;

  int iterationCount = 1;
  while (iterationCount < maxNumberOfIterations) {
    // Compute a(n) and b(n)
    const double an = a(iterationCount, context1, context2);
    const double bn = b(iterationCount, context1, context2);

    // Compute c(n) and d(n)
    double cn = bn + an / cnminus1;
    double dn = bn + an * dnminus1;
    increaseIfTooSmall(&cn);
    increaseIfTooSmall(&dn);
    dn = 1 / dn;

    // Compute delta(n) and h(n)
    const double deltaN = cn * dn;
    hn = hnminus1 * deltaN;

    if (std::isinf(hn) || std::isnan(hn)) {
      return false;
    }
    if (std::fabs(deltaN - 1.0) < 10e-15) {
      break;
    }

    // Update c, d, h
    dnminus1 = dn;
    cnminus1 = cn;
    hnminus1 = hn;
    iterationCount++;
  }

  if (iterationCount >= maxNumberOfIterations) {
    return false;
  }

  *result = hn;
  return true;
}

typedef double (*TermUpdater)(double previousTerm, double index, double d1,
                              double d2, double d3, double d4);
bool InfiniteSeriesEvaluation(double firstTerm, TermUpdater termUpdater,
                              double termLimit, int maxNumberOfIterations,
                              double* result, double context1, double context2,
                              double context3, double context4) {
  double iterationCount = 0.0;
  double currentTerm = firstTerm;
  double sum = currentTerm;
  while (iterationCount < maxNumberOfIterations && !std::isinf(sum) &&
         std::fabs(currentTerm / sum) > termLimit) {
    iterationCount += 1.0;
    currentTerm = termUpdater(currentTerm, iterationCount, context1, context2,
                              context3, context4);
    sum += currentTerm;
  }
  if (iterationCount >= maxNumberOfIterations) {
    return false;
  }
  *result = sum;
  return true;
}

double RegularizedGammaFunction(double s, double x, double epsilon,
                                int maxNumberOfIterations, double* result) {
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
    if (!ContinuedFractionEvaluation(
            [](double index, double s, double x) {
              return index * (s - index);
            },
            [](double index, double s, double x) {
              return (2.0 * index + 1.0) + x - s;
            },
            maxNumberOfIterations, &continuedFractionValue, s, x)) {
      return false;
    }
    *result = 1.0 - std::exp(-x + s * std::log(x) - std::lgamma(s)) *
                        (1.0 / continuedFractionValue);
    return true;
  }

  /* For x < s + 1: Compute using the infinite series representation
   * incompleteGamma(s,x) = 1/gamma(a) * e^-x * x^s * sum(x^n/((s+n)(s+n-1)...)
   */
  double infiniteSeriesValue = 0.0;
  if (!InfiniteSeriesEvaluation(
          1.0 / s,
          [](double previousTerm, double index, double s, double x, double d1,
             double d2) { return previousTerm * x / (s + index); },
          epsilon, maxNumberOfIterations, &infiniteSeriesValue, s, x, 0.0,
          0.0)) {
    return false;
  }
  *result = std::isinf(infiniteSeriesValue)
                ? 1.0
                : std::exp(-x + s * std::log(x) - std::lgamma(s)) *
                      infiniteSeriesValue;
  return true;
}

}  // namespace Poincare
