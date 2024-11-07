#include "affine_regression.h"

#include <poincare/api.h>
#include <poincare/k_tree.h>

namespace Poincare::Regression {
using namespace API;

UserExpression AffineRegression::privateExpression(
    const double* modelCoefficients) const {
  // a*x+b
  return UserExpression::Create(
      KAdd(KMult(KA, "x"_e), KB),
      {.KA = UserExpression::FromDouble(modelCoefficients[0]),
       .KB = UserExpression::FromDouble(modelCoefficients[1])});
}

double AffineRegression::privateEvaluate(
    const CoefficientsType& modelCoefficients, double x) const {
  double slope = modelCoefficients[slopeCoefficientIndex()];
  double yIntercept = modelCoefficients[yInterceptCoefficientIndex()];
  return slope * x + yIntercept;
}

double AffineRegression::levelSet(const double* modelCoefficients, double xMin,
                                  double xMax, double y,
                                  Poincare::Context* context) const {
  double slope = modelCoefficients[slopeCoefficientIndex()];
  double yIntercept = modelCoefficients[yInterceptCoefficientIndex()];
  if (slope == 0) {
    return NAN;
  }
  return (y - yIntercept) / slope;
}

}  // namespace Poincare::Regression
