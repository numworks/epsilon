#include "linear_model.h"
#include "../store.h"
#include <poincare/addition.h>
#include <poincare/multiplication.h>

using namespace Poincare;

namespace Regression {

Poincare::Expression LinearModel::expression(double * modelCoefficients) const {
  if (!m_isApbxForm) {
    return AffineModel::expression(modelCoefficients);
  }
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // a+b*x
  return Addition::Builder({
    Number::DecimalNumber(a),
    Multiplication::Builder({
      Number::DecimalNumber(b),
      Symbol::Builder(k_xSymbol)
    }),
  });
}

void LinearModel::privateFit(Store * store, int series, double * modelCoefficients, Context * context) {
  modelCoefficients[slopeCoefficientIndex()] = store->slope(series);
  modelCoefficients[yInterceptCoefficientIndex()] = store->yIntercept(series);
}

}
