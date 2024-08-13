#include "power_regression.h"

#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

namespace Poincare::Regression {
using namespace API;

UserExpression PowerRegression::privateExpression(
    const double* modelCoefficients) const {
  // a*x^b
  return UserExpression::Create(
      KMult(KA, KPow("x"_e, KB)),
      {.KA = UserExpression::FromDouble(modelCoefficients[0]),
       .KB = UserExpression::FromDouble(modelCoefficients[1])});
}

}  // namespace Poincare::Regression
