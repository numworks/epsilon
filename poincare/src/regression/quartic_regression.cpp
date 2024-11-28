#include "quartic_regression.h"

#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

namespace Poincare::Regression {
using namespace API;

UserExpression QuarticRegression::privateExpression(
    const double* modelCoefficients) const {
  // a*x^4+b*x^3+c*x^2+d*x+e
  return UserExpression::Create(
      KAdd(KMult(KA, KPow("x"_e, 4_e)), KMult(KB, KPow("x"_e, 3_e)),
           KMult(KC, KPow("x"_e, 2_e)), KMult(KD, "x"_e), KE),
      {.KA = UserExpression::FromDouble(modelCoefficients[0]),
       .KB = UserExpression::FromDouble(modelCoefficients[1]),
       .KC = UserExpression::FromDouble(modelCoefficients[2]),
       .KD = UserExpression::FromDouble(modelCoefficients[3]),
       .KE = UserExpression::FromDouble(modelCoefficients[4])});
}

double QuarticRegression::privateEvaluate(const Coefficients& modelCoefficients,
                                          double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double e = modelCoefficients[4];
  return a * x * x * x * x + b * x * x * x + c * x * x + d * x + e;
}

double QuarticRegression::partialDerivate(const Coefficients& modelCoefficients,
                                          int derivateCoefficientIndex,
                                          double x) const {
  switch (derivateCoefficientIndex) {
    case 0:
      // Derivate with respect to a: x^4
      return x * x * x * x;
    case 1:
      // Derivate with respect to b: x^3
      return x * x * x;
    case 2:
      // Derivate with respect to c: x^2
      return x * x;
    case 3:
      // Derivate with respect to d: x
      return x;
    default:
      assert(derivateCoefficientIndex == 4);
      // Derivate with respect to e: 1
      return 1.0;
  };
}

}  // namespace Poincare::Regression
