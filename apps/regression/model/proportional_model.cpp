#include "proportional_model.h"

#include <assert.h>
#include <poincare/multiplication.h>
#include <poincare/print.h>

#include "../store.h"

using namespace Poincare;

namespace Regression {

Expression ProportionalModel::privateExpression(
    double* modelCoefficients) const {
  return Multiplication::Builder(Number::DecimalNumber(modelCoefficients[0]),
                                 Symbol::Builder(k_xSymbol));
}

double ProportionalModel::evaluate(double* modelCoefficients, double x) const {
  return modelCoefficients[0] * x;
}

double ProportionalModel::levelSet(double* modelCoefficients, double xMin,
                                   double xMax, double y,
                                   Poincare::Context* context) {
  const double a = modelCoefficients[0];
  if (a == 0.0) {
    return NAN;
  }
  return y / a;
}

double ProportionalModel::partialDerivate(double* modelCoefficients,
                                          int derivateCoefficientIndex,
                                          double x) const {
  assert(derivateCoefficientIndex == 0);
  // Derivate: x
  return x;
}

}  // namespace Regression
