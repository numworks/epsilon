#include "logarithmic_regression.h"

#include <assert.h>
#include <poincare/k_tree.h>

namespace Poincare::Regression {
using namespace API;

UserExpression LogarithmicRegression::privateExpression(
    const double* modelCoefficients) const {
  // a+b*ln(x)
  return UserExpression::Create(
      KAdd(KA, KMult(KB, KLn("x"_e))),
      {.KA = UserExpression::FromDouble(modelCoefficients[0]),
       .KB = UserExpression::FromDouble(modelCoefficients[1])});
}

}  // namespace Poincare::Regression
