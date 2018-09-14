#include "quadratic_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

LayoutReference QuadraticModel::layout() {
  if (m_layout.isUninitialized()) {
    const LayoutReference layoutChildren[] = {
      CharLayoutRef('a', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      VerticalOffsetLayoutRef(
          CharLayoutRef('2', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('b', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('c', KDText::FontSize::Small),
    };
    m_layout = HorizontalLayoutRef(layoutChildren, 10);
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
      Decimal(a),
      Power(
        Symbol('x'),
        Decimal(2.0))),
    Multiplication(
      Decimal(b),
      Symbol('x')),
    Decimal(c)
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
