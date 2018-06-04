#include "power_model.h"
#include <math.h>
#include <assert.h>

namespace Regression {

double PowerModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a*pow(x,b);
}

double PowerModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (derivateCoefficientIndex == 0) {
    // Derivate: pow(x,b)
    return pow(x,b);
  }
  if (derivateCoefficientIndex == 1) {
    assert(x >= 0);
    /* We assume all xi are positive.
     * For x = 0, a*pow(x,b) = 0, the partial derivate along b is 0
     * For x > 0, a*pow(x,b) = a*exp(b*ln(x)), the partial derivate along b is
     *   ln(x)*a*pow(x,b) */
    return x == 0 ? 0 : a*log(x)*exp(b*x);
  }
  assert(false);
  return 0.0;
}

}
