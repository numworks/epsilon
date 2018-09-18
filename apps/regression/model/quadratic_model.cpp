#include "quadratic_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout QuadraticModel::layout() {
  if (m_layout.isUninitialized()) {
    const Layout layoutChildren[] = {
      CharLayout('a', KDText::FontSize::Small),
      CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayout('X', KDText::FontSize::Small),
      VerticalOffsetLayout(
          CharLayout('2', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayout('+', KDText::FontSize::Small),
      CharLayout('b', KDText::FontSize::Small),
      CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayout('X', KDText::FontSize::Small),
      CharLayout('+', KDText::FontSize::Small),
      CharLayout('c', KDText::FontSize::Small),
    };
    m_layout = HorizontalLayout(layoutChildren, 10);
  }
  return m_layout;
}

Expression QuadraticModel::simplifiedExpression(double * modelCoefficients, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  // a*x^2+b*x+c
  Expression addChildren[] = {
    Multiplication(
      Number::DecimalNumber(a),
      Power(
        Symbol('x'),
        Decimal(2.0))),
    Multiplication(
      Number::DecimalNumber(b),
      Symbol('x')),
    Number::DecimalNumber(c)
  };
  Expression result = Addition(addChildren, 3);
  PoincareHelpers::Simplify(&result, *context);
  return result;
}

double QuadraticModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return a*x*x+b*x+c;
}

double QuadraticModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate: x^2
    return x*x;
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: x
    return x;
  }
  if (derivateCoefficientIndex == 2) {
    // Derivate: 1
    return 1;
  }
  assert(false);
  return 0.0;
}

}
