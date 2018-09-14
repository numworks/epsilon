#include "logistic_model.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

LayoutReference LogisticModel::layout() {
  if (m_layout.isUninitialized()) {
    const LayoutReference exponentLayoutChildren[] = {
      CharLayoutReference('-', KDText::FontSize::Small),
      CharLayoutReference('b', KDText::FontSize::Small),
      CharLayoutReference(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutReference('X', KDText::FontSize::Small)
    };
    const LayoutReference layoutChildren[] = {
      CharLayoutReference('1', KDText::FontSize::Small),
      CharLayoutReference('+', KDText::FontSize::Small),
      CharLayoutReference('a', KDText::FontSize::Small),
      CharLayoutReference(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutReference('e', KDText::FontSize::Small),
      VerticalOffsetLayoutReference(
          HorizontalLayoutReference(exponentLayoutChildren, 4),
          VerticalOffsetLayoutNode::Type::Superscript
        )
    };
    m_layout = FractionLayoutReference(
       CharLayoutReference('c', KDText::FontSize::Small),
       HorizontalLayoutReference(layoutChildren, 6)
      );
  }
  return m_layout;
}

double LogisticModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return c/(1.0+a*exp(-b*x));
}

double LogisticModel::levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  if (a == 0 || b == 0 || c == 0 || y == 0) {
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
