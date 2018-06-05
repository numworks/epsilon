#include "trigonometric_model.h"
#include <math.h>
#include <poincare/preferences.h>
#include <assert.h>

namespace Regression {

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
