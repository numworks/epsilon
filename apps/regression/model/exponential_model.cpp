#include "exponential_model.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

Layout ExponentialModel::layout() {
  if (m_layout.isUninitialized()) {
    const Layout layoutChildren[] = {
      CharLayout('a', KDText::FontSize::Small),
      CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayout('e', KDText::FontSize::Small),
      VerticalOffsetLayout(
          HorizontalLayout(
            CharLayout('b', KDText::FontSize::Small),
            CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
            CharLayout('X', KDText::FontSize::Small)
          ),
          VerticalOffsetLayoutNode::Type::Superscript
        )
    };
    m_layout = HorizontalLayout(layoutChildren, 4);
  }
  return m_layout;
}

double ExponentialModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // a*e^(bx)
  return a*exp(b*x);
}

double ExponentialModel::levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (a == 0 || b == 0 || y/a <= 0) {
    return NAN;
  }
  return log(y/a)/b;
}

double ExponentialModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (derivateCoefficientIndex == 0) {
    // Derivate: exp(b*x)
    return exp(b*x);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: a*x*exp(b*x)
    return a*x*exp(b*x);
  }
  assert(false);
  return 0.0;
}

}
