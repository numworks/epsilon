#include "exponential_model.h"
#include <math.h>
#include <assert.h>
#include "../../poincare/include/poincare_layouts.h"

using namespace Poincare;

namespace Regression {

ExpressionLayout * ExponentialModel::layout() {
  static ExpressionLayout * layout = nullptr;
  if (layout == nullptr) {
    const ExpressionLayout * layoutChildren[] = {
      new CharLayout('a', KDText::FontSize::Small),
      new CharLayout('e', KDText::FontSize::Small),
      new VerticalOffsetLayout(
          new HorizontalLayout(
            new CharLayout('b', KDText::FontSize::Small),
            new CharLayout('X', KDText::FontSize::Small),
            false),
          VerticalOffsetLayout::Type::Superscript,
          false)
    };
    layout = new HorizontalLayout(layoutChildren, 3, false);
  }
  return layout;
}

double ExponentialModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a*exp(b*x);
}

double ExponentialModel::levelSet(double * modelCoefficients, double y) const {
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
