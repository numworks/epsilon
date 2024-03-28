#include "quadratic_model.h"

#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

namespace Regression {

Poincare::Layout QuadraticModel::templateLayout() const {
  return "a·x"_l ^ KSuperscriptL("2"_l) ^ "+b·x+c"_l;
}

Poincare::Expression QuadraticModel::privateExpression(
    double* modelCoefficients) const {
  // a*x^2+b*x+c
  return Poincare::Expression::Create(
      KAdd(KMult(KA, KPow("x"_e, 2_e)), KMult(KB, "x"_e), KC),
      {.KA = Poincare::Expression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::Expression::Builder<double>(modelCoefficients[1]),
       .KC = Poincare::Expression::Builder<double>(modelCoefficients[2])});
}

double QuadraticModel::evaluate(double* modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return a * x * x + b * x + c;
}

double QuadraticModel::partialDerivate(double* modelCoefficients,
                                       int derivateCoefficientIndex,
                                       double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: x^2
    return x * x;
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b: x
    return x;
  }
  assert(derivateCoefficientIndex == 2);
  // Derivate with respect to c: 1
  return 1.0;
}

}  // namespace Regression
