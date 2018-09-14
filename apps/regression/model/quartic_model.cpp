#include "quartic_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

LayoutReference QuarticModel::layout() {
  if (m_layout.isUninitialized()) {
    const LayoutReference layoutChildren[] = {
      CharLayoutRef('a', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      VerticalOffsetLayoutRef(
          CharLayoutRef('4', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('b', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      VerticalOffsetLayoutRef(
          CharLayoutRef('3', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('c', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      VerticalOffsetLayoutRef(
          CharLayoutRef('2', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('d', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('e', KDText::FontSize::Small),
    };
    m_layout = HorizontalLayoutRef(layoutChildren, 20);
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
      Decimal(a),
      Power(
        Symbol('x'),
        Decimal(4.0))),
    // b*x^3
    Multiplication(
      Decimal(b),
      Power(
        Symbol('x'),
        Decimal(3.0))),
    // c*x^2
    Multiplication(
      Decimal(c),
      Power(
        Symbol('x'),
        Decimal(2.0))),
    // d*x
    Multiplication(
      Decimal(d),
      Symbol('x')),
    // e
    Decimal(e)
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
