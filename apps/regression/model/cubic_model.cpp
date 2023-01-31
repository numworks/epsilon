#include "cubic_model.h"
#include "../../shared/poincare_helpers.h"
#include <assert.h>
#include <poincare/addition.h>
#include <poincare/based_integer.h>
#include <poincare/code_point_layout.h>
#include <poincare/decimal.h>
#include <poincare/horizontal_layout.h>
#include <poincare/multiplication.h>
#include <poincare/number.h>
#include <poincare/print.h>
#include <poincare/symbol.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/power.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout CubicModel::layout() {
  if (m_layout.isUninitialized()) {
    m_layout = HorizontalLayout::Builder({
      CodePointLayout::Builder('a'),
      CodePointLayout::Builder(UCodePointMiddleDot),
      CodePointLayout::Builder('x'),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('3'),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript
      ),
      CodePointLayout::Builder('+'),
      CodePointLayout::Builder('b'),
      CodePointLayout::Builder(UCodePointMiddleDot),
      CodePointLayout::Builder('x'),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('2'),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript
      ),
      CodePointLayout::Builder('+'),
      CodePointLayout::Builder('c'),
      CodePointLayout::Builder(UCodePointMiddleDot),
      CodePointLayout::Builder('x'),
      CodePointLayout::Builder('+'),
      CodePointLayout::Builder('d'),
    });
  }
  return m_layout;
}

Expression CubicModel::expression(double * modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  // a*x^3+b*x^2+c*x+d
  return Addition::Builder({
    Multiplication::Builder({
      Number::DecimalNumber(a),
      Power::Builder(
        Symbol::Builder(k_xSymbol),
        BasedInteger::Builder(3)
      )
    }),
    Multiplication::Builder({
      Number::DecimalNumber(b),
      Power::Builder(
        Symbol::Builder(k_xSymbol),
        BasedInteger::Builder(2)
      )
    }),
    Multiplication::Builder({
      Number::DecimalNumber(c),
      Symbol::Builder(k_xSymbol)
    }),
    Number::DecimalNumber(d)
  });
}

double CubicModel::evaluate(double * modelCoefficients, double x) const {
   double a = modelCoefficients[0];
   double b = modelCoefficients[1];
   double c = modelCoefficients[2];
   double d = modelCoefficients[3];
  return a*x*x*x+b*x*x+c*x+d;
}

double CubicModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  switch (derivateCoefficientIndex) {
    case 0:
      // Derivate with respect to a: x^3
      return x*x*x;
    case 1:
      // Derivate with respect to b: x^2
      return x*x;
    case 2:
      // Derivate with respect to c: x
      return x;
    default:
      // Derivate with respect to d: 1
      assert(derivateCoefficientIndex == 3);
      return 1.0;
  };
}

}
