#include "logistic_model.h"
#include <math.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Regression {

Layout LogisticModel::layout() {
  if (m_layout.isUninitialized()) {
    Layout exponentLayoutChildren[] = {
      CodePointLayout::Builder('-', KDFont::SmallFont),
      CodePointLayout::Builder('b', KDFont::SmallFont),
      CodePointLayout::Builder(UCodePointMiddleDot, KDFont::SmallFont),
      CodePointLayout::Builder('X', KDFont::SmallFont)
    };
    Layout layoutChildren[] = {
      CodePointLayout::Builder('1', KDFont::SmallFont),
      CodePointLayout::Builder('+', KDFont::SmallFont),
      CodePointLayout::Builder('a', KDFont::SmallFont),
      CodePointLayout::Builder(UCodePointMiddleDot, KDFont::SmallFont),
      CodePointLayout::Builder('e', KDFont::SmallFont),
      VerticalOffsetLayout::Builder(
          HorizontalLayout::Builder(exponentLayoutChildren, 4),
          VerticalOffsetLayoutNode::Type::Superscript
        )
    };
    m_layout = FractionLayout::Builder(
       CodePointLayout::Builder('c', KDFont::SmallFont),
       HorizontalLayout::Builder(layoutChildren, 6)
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
