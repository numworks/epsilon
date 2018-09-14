#include "cubic_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

LayoutReference CubicModel::layout() {
  if (m_layout.isUninitialized()) {
    const LayoutReference layoutChildren[] = {
      CharLayoutRef('a', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      VerticalOffsetLayoutRef(
          CharLayoutRef('3', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('b', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      VerticalOffsetLayoutRef(
          CharLayoutRef('2', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('c', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('d', KDText::FontSize::Small),
    };
    m_layout = HorizontalLayoutRef(layoutChildren, 15);
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
      Decimal(a),
      Power(
        Symbol('x'),
        Decimal(3.0))),
    Multiplication(
      Decimal(b),
      Power(
        Symbol('x'),
        Decimal(2.0))),
    Multiplication(
      Decimal(c),
      Symbol('x')),
    Decimal(d)
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
