#include "exponential_model.h"
#include <math.h>
#include <assert.h>

namespace Regression {

double ExponentialModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a*exp(b*x);
}

double ExponentialModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (derivateCoefficientIndex == 0) {
    // Derivate: exp(b*x)
    return exp(b*x);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: a*x*exp(b*x)
    return a*x*exp(b*x);
  }
  assert(false);
  return 0.0;
}

}
