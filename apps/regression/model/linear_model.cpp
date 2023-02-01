#include "linear_model.h"
#include "../store.h"
#include <poincare/multiplication.h>

using namespace Poincare;

namespace Regression {

Poincare::Expression LinearModel::privateExpression(double * modelCoefficients) const {
  if (!m_isApbxForm) {
    return AffineModel::privateExpression(modelCoefficients);
  }
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // a+b*x
  return
    AdditionOrSubtractionBuilder(
      Number::DecimalNumber(a),
      Multiplication::Builder({
        Number::DecimalNumber(std::fabs(b)),
        Symbol::Builder(k_xSymbol)
      }),
      b >= 0.0);
}

void LinearModel::privateFit(Store * store, int series, double * modelCoefficients, Context * context) {
  modelCoefficients[slopeCoefficientIndex()] = store->slope(series);
  modelCoefficients[yInterceptCoefficientIndex()] = store->yIntercept(series);
}

}
