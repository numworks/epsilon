#include "quartic_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/decimal.h>
#include <poincare/number.h>
#include <poincare/symbol.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout QuarticModel::layout() {
  if (m_layout.isUninitialized()) {
    constexpr int size = 20;
    Layout layoutChildren[size] = {
      CodePointLayout::Builder('a', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('4', k_layoutFont),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CodePointLayout::Builder('+', k_layoutFont),
      CodePointLayout::Builder('b', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('3', k_layoutFont),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CodePointLayout::Builder('+', k_layoutFont),
      CodePointLayout::Builder('c', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('2', k_layoutFont),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CodePointLayout::Builder('+', k_layoutFont),
      CodePointLayout::Builder('d', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      CodePointLayout::Builder('+', k_layoutFont),
      CodePointLayout::Builder('e', k_layoutFont),
    };
    m_layout = HorizontalLayout::Builder(layoutChildren, size);
  }
  return m_layout;
}

Expression QuarticModel::simplifiedExpression(double * modelCoefficients, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double e = modelCoefficients[4];
  Expression addChildren[] = {
    // a*x^4
    Multiplication::Builder(
      Number::DecimalNumber(a),
      Power::Builder(
        Symbol::Builder('x'),
        Decimal::Builder(4.0))),
    // b*x^3
    Multiplication::Builder(
      Number::DecimalNumber(b),
      Power::Builder(
        Symbol::Builder('x'),
        Decimal::Builder(3.0))),
    // c*x^2
    Multiplication::Builder(
      Number::DecimalNumber(c),
      Power::Builder(
        Symbol::Builder('x'),
        Decimal::Builder(2.0))),
    // d*x
    Multiplication::Builder(
      Number::DecimalNumber(d),
      Symbol::Builder('x')),
    // e
    Number::DecimalNumber(e)
  };
  Expression result = Addition::Builder(addChildren, 5);
  PoincareHelpers::Simplify(&result, *context);
  return result;
}

double QuarticModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double e = modelCoefficients[4];
  return a*x*x*x*x+b*x*x*x+c*x*x+d*x+e;
}

double QuarticModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate: x^4
    return x*x*x*x;
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: x^3
    return x*x*x;
  }
  if (derivateCoefficientIndex == 2) {
    // Derivate: x^2
    return x*x;
  }
  if (derivateCoefficientIndex == 3) {
    // Derivate: x
    return x;
  }
  if (derivateCoefficientIndex == 4) {
    // Derivate: 1
    return 1;
  }
  assert(false);
  return 0.0;
}

}
