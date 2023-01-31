#include "affine_model.h"
#include "../store.h"
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/print.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

Poincare::Expression AffineModel::expression(double * modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // a*x+b
  return Addition::Builder({
    Multiplication::Builder({
      Number::DecimalNumber(a),
      Symbol::Builder(k_xSymbol),
    }),
    Number::DecimalNumber(b)
  });
}

double AffineModel::evaluate(double * modelCoefficients, double x) const {
  double slope = modelCoefficients[slopeCoefficientIndex()];
  double yIntercept = modelCoefficients[yInterceptCoefficientIndex()];
  return slope * x + yIntercept;
}

double AffineModel::levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) {
  double slope = modelCoefficients[slopeCoefficientIndex()];
  double yIntercept = modelCoefficients[yInterceptCoefficientIndex()];
  if (slope == 0) {
    return NAN;
  }
  return (y - yIntercept) / slope;
}

double AffineModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == slopeCoefficientIndex()) {
    // Derivate with respect to slope: x
    return x;
  }
  assert(derivateCoefficientIndex == yInterceptCoefficientIndex());
  // Derivate with respect to yIntercept: 1
  return 1.0;
}

}
