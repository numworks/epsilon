#include "proportional_regression.h"

#include <poincare/k_tree.h>

namespace Poincare::Regression {

using namespace API;

UserExpression ProportionalRegression::privateExpression(
    const double* modelCoefficients) const {
  // a*x
  return UserExpression::Create(
      KMult(KA, "x"_e),
      {.KA = UserExpression::FromDouble(modelCoefficients[0])});
}

double ProportionalRegression::privateEvaluate(
    const CoefficientsType& modelCoefficients, double x) const {
  return modelCoefficients[0] * x;
}

double ProportionalRegression::levelSet(const double* modelCoefficients,
                                        double xMin, double xMax, double y,
                                        Poincare::Context* context) const {
  const double a = modelCoefficients[0];
  if (a == 0.0) {
    return NAN;
  }
  return y / a;
}

double ProportionalRegression::partialDerivate(
    const CoefficientsType& modelCoefficients, int derivateCoefficientIndex,
    double x) const {
  assert(derivateCoefficientIndex == 0);
  // Derivate: x
  return x;
}

}  // namespace Poincare::Regression
