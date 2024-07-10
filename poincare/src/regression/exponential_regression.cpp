#include "exponential_regression.h"

#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

namespace Poincare::Regression {
using namespace API;

Layout ExponentialRegression::templateLayout() const {
  return m_isAbxForm ? "a·b"_l ^ KSuperscriptL("x"_l)
                     : "a·e"_l ^ KSuperscriptL("b·x"_l);
}

UserExpression ExponentialRegression::privateExpression(
    const double* modelCoefficients) const {
  // if m_isAbxForm -> a*b^x, else a*e^bx
  return UserExpression::Create(
      m_isAbxForm ? KMult(KA, KPow(KB, "x"_e))
                  : KMult(KA, KExp(KMult(KB, "x"_e))),
      {.KA = UserExpression::FromDouble(modelCoefficients[0]),
       .KB = UserExpression::FromDouble(modelCoefficients[1])});
}

}  // namespace Poincare::Regression
