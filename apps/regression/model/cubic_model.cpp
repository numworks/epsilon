#include "cubic_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout CubicModel::layout() {
  if (m_layout.isUninitialized()) {
    const Layout layoutChildren[] = {
      CharLayout('a', KDText::FontSize::Small),
      CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayout('X', KDText::FontSize::Small),
      VerticalOffsetLayout(
          CharLayout('3', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout('+', KDText::FontSize::Small),
      CharLayout('b', KDText::FontSize::Small),
      CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayout('X', KDText::FontSize::Small),
      VerticalOffsetLayout(
          CharLayout('2', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout('+', KDText::FontSize::Small),
      CharLayout('c', KDText::FontSize::Small),
      CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayout('X', KDText::FontSize::Small),
      CharLayout('+', KDText::FontSize::Small),
      CharLayout('d', KDText::FontSize::Small),
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
