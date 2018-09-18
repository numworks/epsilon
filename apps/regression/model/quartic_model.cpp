#include "quartic_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout QuarticModel::layout() {
  if (m_layout.isUninitialized()) {
    const Layout layoutChildren[] = {
      CharLayout('a', KDText::FontSize::Small),
      CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayout('X', KDText::FontSize::Small),
      VerticalOffsetLayout(
          CharLayout('4', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout('+', KDText::FontSize::Small),
      CharLayout('b', KDText::FontSize::Small),
      CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayout('X', KDText::FontSize::Small),
      VerticalOffsetLayout(
          CharLayout('3', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout('+', KDText::FontSize::Small),
      CharLayout('c', KDText::FontSize::Small),
      CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayout('X', KDText::FontSize::Small),
      VerticalOffsetLayout(
          CharLayout('2', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout('+', KDText::FontSize::Small),
      CharLayout('d', KDText::FontSize::Small),
      CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayout('X', KDText::FontSize::Small),
      CharLayout('+', KDText::FontSize::Small),
      CharLayout('e', KDText::FontSize::Small),
    };
    m_layout = HorizontalLayout(layoutChildren, 20);
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
    Multiplication(
      Number::DecimalNumber(a),
      Power(
        Symbol('x'),
        Decimal(4.0))),
    // b*x^3
    Multiplication(
      Number::DecimalNumber(b),
      Power(
        Symbol('x'),
        Decimal(3.0))),
    // c*x^2
    Multiplication(
      Number::DecimalNumber(c),
      Power(
        Symbol('x'),
        Decimal(2.0))),
    // d*x
    Multiplication(
      Number::DecimalNumber(d),
      Symbol('x')),
    // e
    Number::DecimalNumber(e)
  };
  Expression result = Addition(addChildren, 5);
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
