#include "linear_model.h"

#include <poincare/expression.h>
#include <poincare/k_tree.h>

#include "../store.h"

namespace Regression {

Poincare::Expression LinearModel::privateExpression(
    double* modelCoefficients) const {
  if (!m_isApbxForm) {
    return AffineModel::privateExpression(modelCoefficients);
  }
  // a+b*x
  return Poincare::Expression::Create(
      KAdd(KA, KMult(KB, "x"_e)),
      {.KA = Poincare::Expression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::Expression::Builder<double>(modelCoefficients[1])});
}

void LinearModel::privateFit(Store* store, int series,
                             double* modelCoefficients,
                             Poincare::Context* context) {
  modelCoefficients[slopeCoefficientIndex()] = store->slope(series);
  modelCoefficients[yInterceptCoefficientIndex()] = store->yIntercept(series);
}

}  // namespace Regression
