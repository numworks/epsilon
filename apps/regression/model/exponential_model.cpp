#include "exponential_model.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

LayoutReference ExponentialModel::layout() {
  static LayoutReference layout;
  if (layout.isUninitialized()) {
    const LayoutReference layoutChildren[] = {
      CharLayoutRef('a', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('e', KDText::FontSize::Small),
      VerticalOffsetLayoutRef(
          HorizontalLayoutRef(
            CharLayoutRef('b', KDText::FontSize::Small),
            CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
            CharLayoutRef('X', KDText::FontSize::Small)
          ),
          VerticalOffsetLayoutNode::Type::Superscript
        )
    };
    layout = HorizontalLayoutRef(layoutChildren, 4);
  }
  return layout;
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
