#include "power_regression.h"

#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include "../store.h"
#include "power_model.h"

namespace Regression {

PowerRegression::PowerRegression() : TransformedRegression() {
  assert(applyLnOnX() == Store::FitsLnX(Regression::Type::Power));
  assert(applyLnOnY() == Store::FitsLnY(Regression::Type::Power));
}

Poincare::Layout PowerRegression::templateLayout() const {
  return "a·x"_l ^ KSuperscriptL("b"_l);
}

Poincare::UserExpression PowerRegression::privateExpression(
    double* modelCoefficients) const {
  // a*x^b
  return Poincare::NewExpression::Create(
      KMult(KA, KPow("x"_e, KB)),
      {.KA = Poincare::NewExpression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::NewExpression::Builder<double>(modelCoefficients[1])});
}

}  // namespace Regression
