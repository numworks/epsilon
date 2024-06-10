#include "linear_regression.h"

#include <poincare/expression.h>
#include <poincare/k_tree.h>

#include "dataset_adapter.h"

namespace Poincare::Regression {

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

void LinearRegression::privateFit(const Series* series,
                                  double* modelCoefficients,
                                  Poincare::Context* context) const {
  DatasetSeriesAdapter dataset(series);
  modelCoefficients[slopeCoefficientIndex()] = dataset.slope();
  modelCoefficients[yInterceptCoefficientIndex()] = dataset.yIntercept();
}

}  // namespace Poincare::Regression
