#include "logistic_model.h"
#include <math.h>
#include <assert.h>
#include "../../poincare/include/poincare_layouts.h"

using namespace Poincare;

namespace Regression {

ExpressionLayout * LogisticModel::layout() {
  static ExpressionLayout * layout = nullptr;
  if (layout == nullptr) {
    const ExpressionLayout * exponentLayoutChildren[] = {
      new CharLayout('-', KDText::FontSize::Small),
      new CharLayout('b', KDText::FontSize::Small),
      new CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      new CharLayout('X', KDText::FontSize::Small)
    };
    const ExpressionLayout * layoutChildren[] = {
      new CharLayout('1', KDText::FontSize::Small),
      new CharLayout('+', KDText::FontSize::Small),
      new CharLayout('a', KDText::FontSize::Small),
      new CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      new CharLayout('e', KDText::FontSize::Small),
      new VerticalOffsetLayout(
          new HorizontalLayout(exponentLayoutChildren, 4, false),
          VerticalOffsetLayout::Type::Superscript,
          false)
    };
    layout = new FractionLayout(
       new CharLayout('c', KDText::FontSize::Small),
       new HorizontalLayout(layoutChildren, 6, false),
       false);
  }
  return layout;
}

double LogisticModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return c/(1.0+a*exp(-b*x));
}

bool LogisticModel::levelSetAvailable(double * modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  if (a == 0 || b == 0 || c == 0) {
    return false;
  }
  return true;
}

double LogisticModel::levelSet(double * modelCoefficients, double y) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  if (!levelSetAvailable(modelCoefficients) || y == 0) {
    return NAN;
  }
  double lnArgument = (c/y - 1)/a;
  if (lnArgument <= 0) {
    return NAN;
  }
  return -log(lnArgument)/b;
}

double LogisticModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double denominator = 1.0+a*exp(-b*x);
  if (derivateCoefficientIndex == 0) {
    // Derivate: exp(-b*x)*(-1 * c/(1.0+a*exp(-b*x))^2)
    return -exp(-b*x) * c/(denominator * denominator);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: (-x)*a*exp(-b*x)*(-1/(1.0+a*exp(-b*x))^2)
    return x*a*exp(-b*x)*c/(denominator * denominator);
  }
  if (derivateCoefficientIndex == 2) {
    // Derivate: (-x)*a*exp(-b*x)*(-1/(1.0+a*exp(-b*x))^2)
    return 1.0/denominator;
  }
  assert(false);
  return 0.0;
}

}
