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

void LinearRegression::privateFit(const Series* series,
                                  double* modelCoefficients,
                                  Context* context) const {
  DatasetSeriesAdapter dataset(series);
  modelCoefficients[slopeCoefficientIndex()] = dataset.slope();
  modelCoefficients[yInterceptCoefficientIndex()] = dataset.yIntercept();
}

}  // namespace Poincare::Regression
