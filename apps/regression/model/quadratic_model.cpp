#include "quadratic_model.h"
#include "../../shared/poincare_helpers.h"
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
#include <poincare/print.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout QuadraticModel::layout() {
  if (m_layout.isUninitialized()) {
    m_layout = HorizontalLayout::Builder({
      CodePointLayout::Builder('a', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('2', k_layoutFont),
        VerticalOffsetLayoutNode::Position::Superscript
      ),
      CodePointLayout::Builder('+', k_layoutFont),
      CodePointLayout::Builder('b', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      CodePointLayout::Builder('+', k_layoutFont),
      CodePointLayout::Builder('c', k_layoutFont),
    });
  }
  return m_layout;
}

int QuadraticModel::buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  return Poincare::Print::safeCustomPrintf(buffer, bufferSize, "%*.*ed·x^2%+*.*ed·x%+*.*ed",
      modelCoefficients[0], displayMode, significantDigits,
      modelCoefficients[1], displayMode, significantDigits,
      modelCoefficients[2], displayMode, significantDigits);
}

double QuadraticModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return a*x*x+b*x+c;
}

double QuadraticModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: x^2
    return x*x;
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b: x
    return x;
  }
  assert(derivateCoefficientIndex == 2);
  // Derivate with respect to c: 1
  return 1.0;
}

Expression QuadraticModel::expression(double * modelCoefficients) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  // a*x^2+b*x+c
  return Addition::Builder({
    Multiplication::Builder({
      Number::DecimalNumber(a),
      Power::Builder(
        Symbol::Builder('x'),
        Decimal::Builder(2.0)
      )
    }),
    Multiplication::Builder({
      Number::DecimalNumber(b),
      Symbol::Builder('x')
    }),
    Number::DecimalNumber(c)
  });
}

}
