#include "proportional_model.h"

#include <poincare/expression.h>
#include <poincare/k_tree.h>

namespace Regression {

Poincare::Expression ProportionalModel::privateExpression(
    double* modelCoefficients) const {
  // a*x
  return Poincare::Expression::Create(
      KMult(KA, "x"_e),
      {.KA = Poincare::Expression::Builder<double>(modelCoefficients[0])});
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
