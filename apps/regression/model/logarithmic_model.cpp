#include "logarithmic_model.h"

#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>

#include "../store.h"

namespace Regression {

LogarithmicModel::LogarithmicModel() : TransformedModel() {
  assert(applyLnOnX() == Store::FitsLnX(Model::Type::Logarithmic));
  assert(applyLnOnY() == Store::FitsLnY(Model::Type::Logarithmic));
}

Poincare::Expression LogarithmicModel::privateExpression(
    double* modelCoefficients) const {
  // a+b*ln(x)
  return Poincare::Expression::Create(
      KAdd(KA, KMult(KB, KLn("x"_e))),
      {.KA = Poincare::Expression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::Expression::Builder<double>(modelCoefficients[1])});
}

}  // namespace Regression
