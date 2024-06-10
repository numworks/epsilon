#include "exponential_regression.h"

#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include "../store.h"
#include "exponential_model.h"

namespace Poincare::Regression {

Poincare::Layout ExponentialRegression::templateLayout() const {
  return m_isAbxForm ? "a·b"_l ^ KSuperscriptL("x"_l)
                     : "a·e"_l ^ KSuperscriptL("b·x"_l);
}

Poincare::UserExpression ExponentialRegression::privateExpression(
    double* modelCoefficients) const {
  // if m_isAbxForm -> a*b^x, else a*e^bx
  return Poincare::NewExpression::Create(
      m_isAbxForm ? KMult(KA, KPow(KB, "x"_e))
                  : KMult(KA, KExp(KMult(KB, "x"_e))),
      {.KA = Poincare::NewExpression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::NewExpression::Builder<double>(modelCoefficients[1])});
}

}  // namespace Poincare::Regression
