#include "quartic_regression.h"

#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

namespace Poincare::Internal {

UserExpression QuarticRegression::privateExpression(
    const double* modelCoefficients) const {
  // a*x^4+b*x^3+c*x^2+d*x+e
  return UserExpression::Create(
      KAdd(KMult(KA, KPow("x"_e, 4_e)), KMult(KB, KPow("x"_e, 3_e)),
           KMult(KC, KPow("x"_e, 2_e)), KMult(KD, "x"_e), KE),
      {.KA = UserExpression::Builder(modelCoefficients[0]),
       .KB = UserExpression::Builder(modelCoefficients[1]),
       .KC = UserExpression::Builder(modelCoefficients[2]),
       .KD = UserExpression::Builder(modelCoefficients[3]),
       .KE = UserExpression::Builder(modelCoefficients[4])});
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

void QuarticRegression::offsetCoefficients(Coefficients& modelCoefficients,
                                           const OffsetSeries* series) const {
  double xo = series->GetXOffset();
  double yo = series->GetYOffset();
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double e = modelCoefficients[4];
  /* Developpement of: y-yo = a(x-xo)^4 + b(x-xo)^3 + c(x-xo)^2 + d(x-xo) + e */
  // yo + e - d * xo + c * xo * xo - b * xo * xo * xo + a * xo * xo * xo * xo;
  modelCoefficients[4] = yo + e + (-d + (c + (-b + a * xo) * xo) * xo) * xo;
  // d - 2 * c * xo + 3 * b * xo * xo - 4 * a * xo * xo * xo;
  modelCoefficients[3] = d + (-2 * c + (3 * b - 4 * a * xo) * xo) * xo;
  modelCoefficients[2] = c - 3 * b * xo + 6 * a * xo * xo;
  modelCoefficients[1] = b - 4 * a * xo;
  /* modelCoefficients[0] = a; */
}

}  // namespace Poincare::Internal
