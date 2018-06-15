#include "quadratic_model.h"
#include <math.h>
#include <assert.h>
#include "../../poincare/include/poincare_layouts.h"

using namespace Poincare;

namespace Regression {

ExpressionLayout * QuadraticModel::layout() {
  static ExpressionLayout * layout = nullptr;
  if (layout == nullptr) {
    const ExpressionLayout * layoutChildren[] = {
      new CharLayout('a', KDText::FontSize::Small),
      new CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      new CharLayout('X', KDText::FontSize::Small),
      new VerticalOffsetLayout(
          new CharLayout('2', KDText::FontSize::Small),
          VerticalOffsetLayout::Type::Superscript,
          false),
      new CharLayout('+', KDText::FontSize::Small),
      new CharLayout('b', KDText::FontSize::Small),
      new CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      new CharLayout('X', KDText::FontSize::Small),
      new CharLayout('+', KDText::FontSize::Small),
      new CharLayout('c', KDText::FontSize::Small),
    };
    layout = new HorizontalLayout(layoutChildren, 10, false);
  }
  return layout;
}

Expression * QuadraticModel::simplifiedExpression(double * modelCoefficients, Poincare::Context * context) {
  if (m_expression != nullptr) {
    delete m_expression;
    m_expression = nullptr;
  }
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  Expression * ax2Expression = new Multiplication(
      new Decimal(a),
      new Power(
        new Symbol('x'),
        new Decimal(2),
        false),
      false);
  Expression * bxExpression = new Multiplication(
    new Decimal(b),
    new Symbol('x'),
    false);
  Expression * cExpression = new Decimal(c);
  Expression * const operands[] = {ax2Expression, bxExpression, cExpression};
  m_expression = new Addition(operands, 3, false);
  Expression::Simplify(&m_expression, *context);
  return m_expression;
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
