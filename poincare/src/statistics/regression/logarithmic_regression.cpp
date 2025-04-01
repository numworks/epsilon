#include "logarithmic_regression.h"

#include <assert.h>
#include <poincare/k_tree.h>

namespace Poincare::Internal {

UserExpression LogarithmicRegression::privateExpression(
    const double* modelCoefficients) const {
  // a+b*ln(x)
  return UserExpression::Create(
      KAdd(KA, KMult(KB, KLnUser("x"_e))),
      {.KA = UserExpression::Builder(modelCoefficients[0]),
       .KB = UserExpression::Builder(modelCoefficients[1])});
}

}  // namespace Poincare::Internal
