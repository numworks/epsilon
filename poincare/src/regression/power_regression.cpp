#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include "../store.h"
#include "power_model.h"

namespace Regression {

PowerModel::PowerModel() : TransformedModel() {
  assert(applyLnOnX() == Store::FitsLnX(Model::Type::Power));
  assert(applyLnOnY() == Store::FitsLnY(Model::Type::Power));
}

Poincare::Layout PowerModel::templateLayout() const {
  return "a·x"_l ^ KSuperscriptL("b"_l);
}

Poincare::UserExpression PowerModel::privateExpression(
    double* modelCoefficients) const {
  // a*x^b
  return Poincare::NewExpression::Create(
      KMult(KA, KPow("x"_e, KB)),
      {.KA = Poincare::NewExpression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::NewExpression::Builder<double>(modelCoefficients[1])});
}

}  // namespace Regression
