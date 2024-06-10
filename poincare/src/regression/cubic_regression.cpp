#include "cubic_regression.h"

#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include "cubic_model.h"

namespace Regression {

Poincare::Layout CubicRegression::templateLayout() const {
  return "a·x"_l ^ KSuperscriptL("3"_l) ^ "+b·x"_l ^ KSuperscriptL("2"_l) ^
         "+c·x+d"_l;
}

Poincare::UserExpression CubicRegression::privateExpression(
    double* modelCoefficients) const {
  // a*x^3+b*x^2+c*x+d
  return Poincare::NewExpression::Create(
      KAdd(KMult(KA, KPow("x"_e, 3_e)), KMult(KB, KPow("x"_e, 2_e)),
           KMult(KC, "x"_e), KD),
      {.KA = Poincare::NewExpression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::NewExpression::Builder<double>(modelCoefficients[1]),
       .KC = Poincare::NewExpression::Builder<double>(modelCoefficients[2]),
       .KD = Poincare::NewExpression::Builder<double>(modelCoefficients[3])});
}

double CubicRegression::evaluate(double* modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  return a * x * x * x + b * x * x + c * x + d;
}

double CubicRegression::partialDerivate(double* modelCoefficients,
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

}  // namespace Regression
