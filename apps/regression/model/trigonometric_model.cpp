#include "trigonometric_model.h"
#include "../../shared/poincare_helpers.h"
#include "../../poincare/include/poincare_layouts.h"
#include <math.h>
#include <poincare/preferences.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

ExpressionLayout * TrigonometricModel::layout() {
  static ExpressionLayout * layout = nullptr;
  if (layout == nullptr) {
    const ExpressionLayout * layoutChildren[] = {
      CharLayoutRef('a', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('s', KDText::FontSize::Small),
      CharLayoutRef('i', KDText::FontSize::Small),
      CharLayoutRef('n', KDText::FontSize::Small),
      CharLayoutRef('(', KDText::FontSize::Small),
      CharLayoutRef('b', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('c', KDText::FontSize::Small),
      CharLayoutRef(')', KDText::FontSize::Small),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('d', KDText::FontSize::Small)
    };
    layout = HorizontalLayoutRef(layoutChildren, 14, false);
  }
  return layout;
}

Expression * TrigonometricModel::simplifiedExpression(double * modelCoefficients, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  Expression * aExpression = new Decimal(a);
  Expression * sinExpression = new Sine(
      new Addition(
        new Multiplication(
          new Decimal(b),
          new Symbol('x'),
          false),
        new Decimal(c),
        false),
      false);
  Expression * asinExpression = new Multiplication(aExpression, sinExpression, false);
  Expression * dExpression = new Decimal(d);
  Expression * result = new Addition(asinExpression, dExpression, false);
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
