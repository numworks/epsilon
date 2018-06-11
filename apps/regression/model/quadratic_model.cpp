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

double QuadraticModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return a*x*x+b*x+c;
}

double QuadraticModel::levelSet(double * modelCoefficients, double y) const {
  return NAN;
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
