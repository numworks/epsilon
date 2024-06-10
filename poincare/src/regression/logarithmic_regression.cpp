#include "logarithmic_regression.h"

#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>

#include "../store.h"
#include "logarithmic_model.h"

namespace Regression {

LogarithmicRegression::LogarithmicRegression() : TransformedRegression() {
  assert(applyLnOnX() == Store::FitsLnX(Regression::Type::Logarithmic));
  assert(applyLnOnY() == Store::FitsLnY(Regression::Type::Logarithmic));
}

Poincare::UserExpression LogarithmicRegression::privateExpression(
    double* modelCoefficients) const {
  // a+b*ln(x)
  return Poincare::NewExpression::Create(
      KAdd(KA, KMult(KB, KLn("x"_e))),
      {.KA = Poincare::NewExpression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::NewExpression::Builder<double>(modelCoefficients[1])});
}

}  // namespace Regression
