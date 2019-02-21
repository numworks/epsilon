#include "quartic_model.h"
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

Layout QuarticModel::layout() {
  if (m_layout.isUninitialized()) {
    const Layout layoutChildren[] = {
      CharLayout::Builder('a', KDFont::SmallFont),
      CharLayout::Builder(Ion::Charset::MiddleDot, KDFont::SmallFont),
      CharLayout::Builder('X', KDFont::SmallFont),
      VerticalOffsetLayout::Builder(
          CharLayout::Builder('4', KDFont::SmallFont),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout::Builder('+', KDFont::SmallFont),
      CharLayout::Builder('b', KDFont::SmallFont),
      CharLayout::Builder(Ion::Charset::MiddleDot, KDFont::SmallFont),
      CharLayout::Builder('X', KDFont::SmallFont),
      VerticalOffsetLayout::Builder(
          CharLayout::Builder('3', KDFont::SmallFont),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout::Builder('+', KDFont::SmallFont),
      CharLayout::Builder('c', KDFont::SmallFont),
      CharLayout::Builder(Ion::Charset::MiddleDot, KDFont::SmallFont),
      CharLayout::Builder('X', KDFont::SmallFont),
      VerticalOffsetLayout::Builder(
          CharLayout::Builder('2', KDFont::SmallFont),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout::Builder('+', KDFont::SmallFont),
      CharLayout::Builder('d', KDFont::SmallFont),
      CharLayout::Builder(Ion::Charset::MiddleDot, KDFont::SmallFont),
      CharLayout::Builder('X', KDFont::SmallFont),
      CharLayout::Builder('+', KDFont::SmallFont),
      CharLayout::Builder('e', KDFont::SmallFont),
    };
    m_layout = HorizontalLayout::Builder(layoutChildren, 20);
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
