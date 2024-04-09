#include "power_model.h"

#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include "../store.h"

namespace Regression {

PowerModel::PowerModel() : TransformedModel() {
  assert(applyLnOnX() == Store::FitsLnX(Model::Type::Power));
  assert(applyLnOnY() == Store::FitsLnY(Model::Type::Power));
}

Poincare::Layout PowerModel::templateLayout() const {
  return "a·x"_l ^ KSuperscriptL("b"_l);
}

Poincare::Expression PowerModel::privateExpression(
    double* modelCoefficients) const {
  // a*x^b
  return Poincare::Expression::Create(
      KMult(KA, KPow("x"_e, KB)),
      {.KA = Poincare::Expression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::Expression::Builder<double>(modelCoefficients[1])});
}

}  // namespace Regression
