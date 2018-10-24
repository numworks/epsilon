#include "trigonometric_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/number.h>
#include <poincare/symbol.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/sine.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout TrigonometricModel::layout() {
  if (m_layout.isUninitialized()) {
    const Layout layoutChildren[] = {
      CharLayout('a', KDFont::SmallFont),
      CharLayout(Ion::Charset::MiddleDot, KDFont::SmallFont),
      CharLayout('s', KDFont::SmallFont),
      CharLayout('i', KDFont::SmallFont),
      CharLayout('n', KDFont::SmallFont),
      CharLayout('(', KDFont::SmallFont),
      CharLayout('b', KDFont::SmallFont),
      CharLayout(Ion::Charset::MiddleDot, KDFont::SmallFont),
      CharLayout('X', KDFont::SmallFont),
      CharLayout('+', KDFont::SmallFont),
      CharLayout('c', KDFont::SmallFont),
      CharLayout(')', KDFont::SmallFont),
      CharLayout('+', KDFont::SmallFont),
      CharLayout('d', KDFont::SmallFont)
    };
    m_layout = HorizontalLayout(layoutChildren, 14);
  }
  return m_layout;
}

Expression TrigonometricModel::simplifiedExpression(double * modelCoefficients, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  // a*sin(bx+c)+d
  Expression result =
    Addition(
      Multiplication(
        Number::DecimalNumber(a),
        Sine::Builder(
          Addition(
            Multiplication(
              Number::DecimalNumber(b),
              Symbol('x')),
            Number::DecimalNumber(c)))),
      Number::DecimalNumber(d));
  PoincareHelpers::Simplify(&result, *context);
  return result;
}

double TrigonometricModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double radianX = Poincare::Preferences::sharedPreferences()->angleUnit() == Poincare::Preferences::AngleUnit::Radian ? x : x * M_PI/180.0;
  return a*sin(b*radianX+c)+d;
}

double TrigonometricModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double radianX = Poincare::Preferences::sharedPreferences()->angleUnit() == Poincare::Preferences::AngleUnit::Radian ? x : x * M_PI/180.0;
  if (derivateCoefficientIndex == 0) {
    // Derivate: sin(b*x+c)
    return sin(b*radianX+c);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: x*a*cos(b*x+c);
    return radianX*a*cos(b*radianX+c);
  }
  if (derivateCoefficientIndex == 2) {
    // Derivate: a*cos(b*x+c)
    return a*cos(b*radianX+c);
  }
  if (derivateCoefficientIndex == 3) {
    // Derivate: 1
    return 1.0;
  }
  assert(false);
  return 0.0;
}

}
