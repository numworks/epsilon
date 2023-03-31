#include "power_model.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/print.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

#include "../store.h"

using namespace Poincare;

namespace Regression {

Layout PowerModel::templateLayout() const {
  return HorizontalLayout::Builder(
      {CodePointLayout::Builder('a'),
       CodePointLayout::Builder(UCodePointMiddleDot),
       CodePointLayout::Builder('x'),
       VerticalOffsetLayout::Builder(
           CodePointLayout::Builder('b'),
           VerticalOffsetLayoutNode::VerticalPosition::Superscript)});
}

Expression PowerModel::privateExpression(double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // a*x^b
  return Multiplication::Builder(
      Number::DecimalNumber(a),
      Power::Builder(Symbol::Builder(k_xSymbol), Number::DecimalNumber(b)));
}

}  // namespace Regression
