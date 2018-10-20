#include "cubic_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <assert.h>
#include <poincare/char_layout.h>
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

Layout CubicModel::layout() {
  if (m_layout.isUninitialized()) {
    const Layout layoutChildren[] = {
      CharLayout('a', KDFont::SmallFont),
      CharLayout(Ion::Charset::MiddleDot, KDFont::SmallFont),
      CharLayout('X', KDFont::SmallFont),
      VerticalOffsetLayout(
          CharLayout('3', KDFont::SmallFont),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout('+', KDFont::SmallFont),
      CharLayout('b', KDFont::SmallFont),
      CharLayout(Ion::Charset::MiddleDot, KDFont::SmallFont),
      CharLayout('X', KDFont::SmallFont),
      VerticalOffsetLayout(
          CharLayout('2', KDFont::SmallFont),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout('+', KDFont::SmallFont),
      CharLayout('c', KDFont::SmallFont),
      CharLayout(Ion::Charset::MiddleDot, KDFont::SmallFont),
      CharLayout('X', KDFont::SmallFont),
      CharLayout('+', KDFont::SmallFont),
      CharLayout('d', KDFont::SmallFont),
    };
    m_layout = HorizontalLayout(layoutChildren, 15);
  }
  return m_layout;
}

Expression CubicModel::simplifiedExpression(double * modelCoefficients, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  Expression addChildren[] = {
    Multiplication(
      Number::DecimalNumber(a),
      Power(
        Symbol('x'),
        Decimal(3.0))),
    Multiplication(
      Number::DecimalNumber(b),
      Power(
        Symbol('x'),
        Decimal(2.0))),
    Multiplication(
      Number::DecimalNumber(c),
      Symbol('x')),
    Number::DecimalNumber(d)
    };
  // a*x^3+b*x^2+c*x+d
  Expression result = Addition(addChildren, 4);
  PoincareHelpers::Simplify(&result, *context);
  return result;
}

double CubicModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  return a*x*x*x+b*x*x+c*x+d;
}

double CubicModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate: x^3
    return x*x*x;
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: x^2
    return x*x;
  }
  if (derivateCoefficientIndex == 2) {
    // Derivate: x
    return x;
  }
  if (derivateCoefficientIndex == 3) {
    // Derivate: 1
    return 1;
  }
  assert(false);
  return 0.0;
}

}
