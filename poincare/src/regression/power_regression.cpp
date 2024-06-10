#include "power_regression.h"

#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

namespace Poincare::Regression {

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

}  // namespace Poincare::Regression
