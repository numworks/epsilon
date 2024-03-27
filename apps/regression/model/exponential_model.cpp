#include "exponential_model.h"

#include <assert.h>
#include <poincare/constant.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/print.h>

#include <cmath>

#include "../store.h"

using namespace Poincare;
using namespace PoincareJ;
using namespace Shared;

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

Layout ExponentialModel::templateLayout() const {
  return m_isAbxForm ? "a·b"_l ^ KSuperscriptL("x"_l)
                     : "a·e"_l ^ KSuperscriptL("b·x"_l);
}

Poincare::Expression ExponentialModel::privateExpression(
    double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // if m_isAbxForm -> a*b^x, else a*e^bx
  return m_isAbxForm ? Multiplication::Builder(
                           Number::DecimalNumber(a),
                           Power::Builder(Number::DecimalNumber(b),
                                          Symbol::Builder(k_xSymbol)))
                     : Multiplication::Builder(
                           Number::DecimalNumber(a),
                           Power::Builder(Constant::ExponentialEBuilder(),
                                          Multiplication::Builder(
                                              Number::DecimalNumber(b),
                                              Symbol::Builder(k_xSymbol))));
}

}  // namespace Regression
