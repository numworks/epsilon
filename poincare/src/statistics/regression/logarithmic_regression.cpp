#include "logarithmic_regression.h"

#include <assert.h>
#include <poincare/k_tree.h>

namespace Poincare::Internal {
using namespace API;

UserExpression LogarithmicRegression::privateExpression(
    const double* modelCoefficients) const {
  // a+b*ln(x)
  return UserExpression::Create(
      KAdd(KA, KMult(KB, KLnUser("x"_e))),
      {.KA = UserExpression::FromDouble(modelCoefficients[0]),
       .KB = UserExpression::FromDouble(modelCoefficients[1])});
}

}  // namespace Poincare::Internal
