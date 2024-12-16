#include "quadratic_regression.h"

#include <poincare/k_tree.h>
#include <poincare/layout.h>

namespace Poincare::Regression {
using namespace API;

UserExpression QuadraticRegression::privateExpression(
    const double* modelCoefficients) const {
  // a*x^2+b*x+c
  return UserExpression::Create(
      KAdd(KMult(KA, KPow("x"_e, 2_e)), KMult(KB, "x"_e), KC),
      {.KA = UserExpression::FromDouble(modelCoefficients[0]),
       .KB = UserExpression::FromDouble(modelCoefficients[1]),
       .KC = UserExpression::FromDouble(modelCoefficients[2])});
}

double QuadraticRegression::privateEvaluate(
    const Coefficients& modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return a * x * x + b * x + c;
}

double QuadraticRegression::partialDerivate(
    const Coefficients& modelCoefficients, int derivateCoefficientIndex,
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

}  // namespace Poincare::Regression
