#include "affine_regression.h"

#include <poincare/expression.h>
#include <poincare/k_tree.h>

#include "affine_model.h"

namespace Regression {

Poincare::UserExpression AffineRegression::privateExpression(
    double* modelCoefficients) const {
  // a*x+b
  return Poincare::NewExpression::Create(
      KAdd(KMult(KA, "x"_e), KB),
      {.KA = Poincare::NewExpression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::NewExpression::Builder<double>(modelCoefficients[1])});
}

double AffineRegression::evaluate(double* modelCoefficients, double x) const {
  double slope = modelCoefficients[slopeCoefficientIndex()];
  double yIntercept = modelCoefficients[yInterceptCoefficientIndex()];
  return slope * x + yIntercept;
}

double AffineRegression::levelSet(double* modelCoefficients, double xMin,
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
