#include "cubic_model.h"
#include <math.h>
#include <assert.h>
#include "../../poincare/include/poincare_layouts.h"

using namespace Poincare;

namespace Regression {

ExpressionLayout * CubicModel::Layout() {
  static ExpressionLayout * layout = nullptr;
  if (layout == nullptr) {
    const ExpressionLayout * layoutChildren[] = {
      new CharLayout('a', KDText::FontSize::Small),
      new CharLayout('X', KDText::FontSize::Small),
      new VerticalOffsetLayout(
          new CharLayout('3', KDText::FontSize::Small),
          VerticalOffsetLayout::Type::Superscript,
          false),
      new CharLayout('+', KDText::FontSize::Small),
      new CharLayout('b', KDText::FontSize::Small),
      new CharLayout('X', KDText::FontSize::Small),
      new VerticalOffsetLayout(
          new CharLayout('2', KDText::FontSize::Small),
          VerticalOffsetLayout::Type::Superscript,
          false),
      new CharLayout('+', KDText::FontSize::Small),
      new CharLayout('c', KDText::FontSize::Small),
      new CharLayout('X', KDText::FontSize::Small),
      new CharLayout('+', KDText::FontSize::Small),
      new CharLayout('d', KDText::FontSize::Small),
    };
    layout = new HorizontalLayout(layoutChildren, 12, false);
  }
  return layout;
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
