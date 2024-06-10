#include "linear_regression.h"

#include <poincare/expression.h>
#include <poincare/k_tree.h>

#include "../store.h"
#include "linear_model.h"

namespace Regression {

Poincare::UserExpression LinearRegression::privateExpression(
    double* modelCoefficients) const {
  if (!m_isApbxForm) {
    return AffineRegression::privateExpression(modelCoefficients);
  }
  // a+b*x
  return Poincare::NewExpression::Create(
      KAdd(KA, KMult(KB, "x"_e)),
      {.KA = Poincare::NewExpression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::NewExpression::Builder<double>(modelCoefficients[1])});
}

void LinearRegression::privateFit(Store* store, int series,
                                  double* modelCoefficients,
                                  Poincare::Context* context) {
  modelCoefficients[slopeCoefficientIndex()] = store->slope(series);
  modelCoefficients[yInterceptCoefficientIndex()] = store->yIntercept(series);
}

}  // namespace Regression
