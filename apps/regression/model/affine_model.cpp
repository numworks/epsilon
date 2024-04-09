#include "affine_model.h"

#include <poincare/expression.h>
#include <poincare/k_tree.h>

namespace Regression {

Poincare::Expression AffineModel::privateExpression(
    double* modelCoefficients) const {
  // a*x+b
  return Poincare::Expression::Create(
      KAdd(KMult(KA, "x"_e), KB),
      {.KA = Poincare::Expression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::Expression::Builder<double>(modelCoefficients[1])});
}

double AffineModel::evaluate(double* modelCoefficients, double x) const {
  double slope = modelCoefficients[slopeCoefficientIndex()];
  double yIntercept = modelCoefficients[yInterceptCoefficientIndex()];
  return slope * x + yIntercept;
}

double AffineModel::levelSet(double* modelCoefficients, double xMin,
                             double xMax, double y,
                             Poincare::Context* context) {
  double slope = modelCoefficients[slopeCoefficientIndex()];
  double yIntercept = modelCoefficients[yInterceptCoefficientIndex()];
  if (slope == 0) {
    return NAN;
  }
  return (y - yIntercept) / slope;
}

}  // namespace Regression
