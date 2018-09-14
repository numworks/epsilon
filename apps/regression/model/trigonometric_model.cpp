#include "trigonometric_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <poincare/preferences.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

LayoutReference TrigonometricModel::layout() {
  if (m_layout.isUninitialized()) {
    const LayoutReference layoutChildren[] = {
      CharLayoutReference('a', KDText::FontSize::Small),
      CharLayoutReference(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutReference('s', KDText::FontSize::Small),
      CharLayoutReference('i', KDText::FontSize::Small),
      CharLayoutReference('n', KDText::FontSize::Small),
      CharLayoutReference('(', KDText::FontSize::Small),
      CharLayoutReference('b', KDText::FontSize::Small),
      CharLayoutReference(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutReference('X', KDText::FontSize::Small),
      CharLayoutReference('+', KDText::FontSize::Small),
      CharLayoutReference('c', KDText::FontSize::Small),
      CharLayoutReference(')', KDText::FontSize::Small),
      CharLayoutReference('+', KDText::FontSize::Small),
      CharLayoutReference('d', KDText::FontSize::Small)
    };
    m_layout = HorizontalLayoutReference(layoutChildren, 14);
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
        Decimal(a),
        Sine(
          Addition(
            Multiplication(
              Decimal(b),
              Symbol('x')),
            Decimal(c)))),
      Decimal(d));
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
