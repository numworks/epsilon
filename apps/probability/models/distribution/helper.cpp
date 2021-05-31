#include "helper.h"
#include "distribution.h"
#include <cmath>
#include <float.h>
#include <assert.h>

void increaseIfTooSmall(double * d) {
  assert(!std::isinf(*d) && !std::isnan(*d));
  const double small = 1e-50;
  if (std::fabs(*d) < small) {
    *d = small;
  }
}

bool Helper::ContinuedFractionEvaluation(ValueForIndex a, ValueForIndex b, int maxNumberOfIterations, double * result, double context1, double context2) {
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

bool Helper::InfiniteSeriesEvaluation(double firstTerm, TermUpdater termUpdater, double termLimit, int maxNumberOfIterations, double * result, double context1, double context2, double context3, double context4) {
  double iterationCount = 0.0;
  double currentTerm = firstTerm;
  double sum = currentTerm;
  while (iterationCount < maxNumberOfIterations
      && !std::isinf(sum)
      && std::fabs(currentTerm/sum) > termLimit)
  {
    iterationCount+= 1.0;
    currentTerm = termUpdater(currentTerm, iterationCount, context1, context2, context3, context4);
    sum+= currentTerm;
  }
  if (iterationCount >= maxNumberOfIterations) {
    return false;
  }
  *result = sum;
  return true;
}
