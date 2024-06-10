#include "quadratic_regression.h"

#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include "quadratic_model.h"

namespace Regression {

Poincare::Layout QuadraticRegression::templateLayout() const {
  return "a·x"_l ^ KSuperscriptL("2"_l) ^ "+b·x+c"_l;
}

Poincare::UserExpression QuadraticRegression::privateExpression(
    double* modelCoefficients) const {
  // a*x^2+b*x+c
  return Poincare::NewExpression::Create(
      KAdd(KMult(KA, KPow("x"_e, 2_e)), KMult(KB, "x"_e), KC),
      {.KA = Poincare::NewExpression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::NewExpression::Builder<double>(modelCoefficients[1]),
       .KC = Poincare::NewExpression::Builder<double>(modelCoefficients[2])});
}

double QuadraticRegression::evaluate(double* modelCoefficients,
                                     double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return a * x * x + b * x + c;
}

double QuadraticRegression::partialDerivate(double* modelCoefficients,
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
