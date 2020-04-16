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
    m_layout = HorizontalLayout::Builder({
      CodePointLayout::Builder('a', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('4', k_layoutFont),
        VerticalOffsetLayoutNode::Position::Superscript
      ),
      CodePointLayout::Builder('+', k_layoutFont),
      CodePointLayout::Builder('b', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('3', k_layoutFont),
        VerticalOffsetLayoutNode::Position::Superscript
      ),
      CodePointLayout::Builder('+', k_layoutFont),
      CodePointLayout::Builder('c', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('2', k_layoutFont),
        VerticalOffsetLayoutNode::Position::Superscript
      ),
      CodePointLayout::Builder('+', k_layoutFont),
      CodePointLayout::Builder('d', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      CodePointLayout::Builder('+', k_layoutFont),
      CodePointLayout::Builder('e', k_layoutFont),
    });
  }
  return m_layout;
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

Expression QuarticModel::expression(double * modelCoefficients) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double e = modelCoefficients[4];
  // a*x^4+b*x^3+c*x^2+d*x+e
  return Addition::Builder({
    Multiplication::Builder({
      Number::DecimalNumber(a),
      Power::Builder(
        Symbol::Builder('x'),
        Decimal::Builder(4.0)
      )
    }),
    Multiplication::Builder({
      Number::DecimalNumber(b),
      Power::Builder(
        Symbol::Builder('x'),
        Decimal::Builder(3.0)
      )
    }),
    Multiplication::Builder({
      Number::DecimalNumber(c),
      Power::Builder(
        Symbol::Builder('x'),
        Decimal::Builder(2.0)
      )
    }),
    Multiplication::Builder({
      Number::DecimalNumber(d),
      Symbol::Builder('x')
    }),
    Number::DecimalNumber(e)
  });
}

}
