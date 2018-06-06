#include "trigonometric_model.h"
#include <math.h>
#include <poincare/preferences.h>
#include <poincare/layout_engine.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

ExpressionLayout * TrigonometricModel::Layout() {
  static ExpressionLayout * layout = LayoutEngine::createStringLayout("asin(bX+c)", 10, KDText::FontSize::Small);
  return layout;
}

double TrigonometricModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double radianX = Poincare::Preferences::sharedPreferences()->angleUnit() == Poincare::Expression::AngleUnit::Radian ? x : x * M_PI/180.0;
  return a*sin(b*radianX+c);
}

double TrigonometricModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double radianX = Poincare::Preferences::sharedPreferences()->angleUnit() == Poincare::Expression::AngleUnit::Radian ? x : x * M_PI/180.0;
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
  assert(false);
  return 0.0;
}

}
