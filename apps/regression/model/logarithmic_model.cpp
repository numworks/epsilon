#include "logarithmic_model.h"

#include <assert.h>
#include <poincare/multiplication.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/print.h>

#include <cmath>

#include "../store.h"

using namespace Poincare;

namespace Regression {

Poincare::Expression LogarithmicModel::privateExpression(
    double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // a+b*ln(x)
  return AdditionOrSubtractionBuilder(
      Number::DecimalNumber(a),
      Multiplication::Builder(
          Number::DecimalNumber(std::fabs(b)),
          NaperianLogarithm::Builder(Symbol::Builder(k_xSymbol))),
      b >= 0.0);
}

}  // namespace Regression
