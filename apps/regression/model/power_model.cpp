#include "power_model.h"

#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/print.h>

#include <cmath>

#include "../store.h"

using namespace Poincare;
using namespace PoincareJ;

namespace Regression {

PowerModel::PowerModel() : TransformedModel() {
  assert(applyLnOnX() == Store::FitsLnX(Model::Type::Power));
  assert(applyLnOnY() == Store::FitsLnY(Model::Type::Power));
}

Layout PowerModel::templateLayout() const {
  return "a·x"_l ^ KSuperscriptL("b"_l);
}

Expression PowerModel::privateExpression(double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // a*x^b
  return Multiplication::Builder(
      Number::DecimalNumber(a),
      Power::Builder(Symbol::Builder(k_xSymbol), Number::DecimalNumber(b)));
}

}  // namespace Regression
