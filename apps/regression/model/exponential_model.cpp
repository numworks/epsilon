#include "exponential_model.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/constant.h>
#include <poincare/horizontal_layout.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/print.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

#include "../store.h"

using namespace Poincare;
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
  Layout base;
  Layout exponent;
  if (m_isAbxForm) {
    base = CodePointLayout::Builder('b'),
    exponent = CodePointLayout::Builder('x');
  } else {
    base = CodePointLayout::Builder('e'),
    exponent = HorizontalLayout::Builder(
        {CodePointLayout::Builder('b'),
         CodePointLayout::Builder(UCodePointMiddleDot),
         CodePointLayout::Builder('x')});
  }
  return HorizontalLayout::Builder(
      {CodePointLayout::Builder('a'),
       CodePointLayout::Builder(UCodePointMiddleDot), base,
       VerticalOffsetLayout::Builder(
           exponent, VerticalOffsetLayoutNode::VerticalPosition::Superscript)});
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
