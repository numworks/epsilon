#include "linear_regression.h"

#include <poincare/k_tree.h>

#include "dataset_adapter.h"

namespace Poincare::Regression {
using namespace API;

UserExpression LinearRegression::privateExpression(
    const double* modelCoefficients) const {
  if (!m_isApbxForm) {
    return AffineRegression::privateExpression(modelCoefficients);
  }
  // a+b*x
  return UserExpression::Create(
      KAdd(KA, KMult(KB, "x"_e)),
      {.KA = UserExpression::FromDouble(modelCoefficients[0]),
       .KB = UserExpression::FromDouble(modelCoefficients[1])});
}

Regression::CoefficientsType LinearRegression::privateFit(
    const Series* series, Context* context) const {
  return {series->slope(), series->yIntercept()};
}

}  // namespace Poincare::Regression
