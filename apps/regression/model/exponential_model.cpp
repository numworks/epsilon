#include "exponential_model.h"

#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include "../store.h"

namespace Regression {

ExponentialModel::ExponentialModel(bool isAbxForm)
    : TransformedModel(), m_isAbxForm(isAbxForm) {
  assert(applyLnOnX() == Store::FitsLnX(isAbxForm
                                            ? Model::Type::ExponentialAbx
                                            : Model::Type::ExponentialAebx));
  assert(applyLnOnY() == Store::FitsLnY(isAbxForm
                                            ? Model::Type::ExponentialAbx
                                            : Model::Type::ExponentialAebx));
}

Poincare::Layout ExponentialModel::templateLayout() const {
  return m_isAbxForm ? "a·b"_l ^ KSuperscriptL("x"_l)
                     : "a·e"_l ^ KSuperscriptL("b·x"_l);
}

Poincare::Expression ExponentialModel::privateExpression(
    double* modelCoefficients) const {
  // if m_isAbxForm -> a*b^x, else a*e^bx
  return Poincare::Expression::Create(
      m_isAbxForm ? KMult(KA, KPow(KB, "x"_e))
                  : KMult(KA, KExp(KMult(KB, "x"_e))),
      {.KA = Poincare::Expression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::Expression::Builder<double>(modelCoefficients[1])});
}

}  // namespace Regression
