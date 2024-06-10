#include "proportional_regression.h"

#include <poincare/expression.h>
#include <poincare/k_tree.h>

#include "proportional_model.h"

namespace Regression {

Poincare::UserExpression ProportionalRegression::privateExpression(
    double* modelCoefficients) const {
  // a*x
  return Poincare::NewExpression::Create(
      KMult(KA, "x"_e),
      {.KA = Poincare::NewExpression::Builder<double>(modelCoefficients[0])});
}

double ProportionalRegression::evaluate(double* modelCoefficients,
                                        double x) const {
  return modelCoefficients[0] * x;
}

double ProportionalRegression::levelSet(double* modelCoefficients, double xMin,
                                        double xMax, double y,
                                        Poincare::Context* context) {
  const double a = modelCoefficients[0];
  if (a == 0.0) {
    return NAN;
  }
  return y / a;
}

double ProportionalRegression::partialDerivate(double* modelCoefficients,
                                               int derivateCoefficientIndex,
                                               double x) const {
  assert(derivateCoefficientIndex == 0);
  // Derivate: x
  return x;
}

}  // namespace Regression
