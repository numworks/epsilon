#include "cubic_regression.h"

#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

namespace Poincare::Regression {
using namespace API;

UserExpression CubicRegression::privateExpression(
    const double* modelCoefficients) const {
  // a*x^3+b*x^2+c*x+d
  return UserExpression::Create(
      KAdd(KMult(KA, KPow("x"_e, 3_e)), KMult(KB, KPow("x"_e, 2_e)),
           KMult(KC, "x"_e), KD),
      {.KA = UserExpression::FromDouble(modelCoefficients[0]),
       .KB = UserExpression::FromDouble(modelCoefficients[1]),
       .KC = UserExpression::FromDouble(modelCoefficients[2]),
       .KD = UserExpression::FromDouble(modelCoefficients[3])});
}

double CubicRegression::privateEvaluate(const Coefficients& modelCoefficients,
                                        double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  return a * x * x * x + b * x * x + c * x + d;
}

double CubicRegression::partialDerivate(const Coefficients& modelCoefficients,
                                        int derivateCoefficientIndex,
                                        double x) const {
  switch (derivateCoefficientIndex) {
    case 0:
      // Derivate with respect to a: x^3
      return x * x * x;
    case 1:
      // Derivate with respect to b: x^2
      return x * x;
    case 2:
      // Derivate with respect to c: x
      return x;
    default:
      // Derivate with respect to d: 1
      assert(derivateCoefficientIndex == 3);
      return 1.0;
  };
}

}  // namespace Poincare::Regression
