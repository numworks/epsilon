#include "trigonometric_model.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/number.h>
#include <poincare/power.h>
#include <poincare/preferences.h>
#include <poincare/sine.h>
#include <poincare/symbol.h>
#include <math.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

static double toRadians(Poincare::Preferences::AngleUnit angleUnit) {
  switch (Poincare::Preferences::sharedPreferences()->angleUnit()) {
    case Poincare::Preferences::AngleUnit::Degree:
      return M_PI/180.0;
    case Poincare::Preferences::AngleUnit::Gradian:
      return M_PI/200.0;
    default:
      return 1;
  }
}

Layout TrigonometricModel::layout() {
  if (m_layout.isUninitialized()) {
    const char * s = "a·sin(b·X+c)+d";
    m_layout = LayoutHelper::String(s, strlen(s), k_layoutFont);
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
    Addition::Builder(
      Multiplication::Builder(
        Number::DecimalNumber(a),
        Sine::Builder(
          Addition::Builder(
            Multiplication::Builder(
              Number::DecimalNumber(b),
              Symbol::Builder('x')),
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
  double radianX = x * toRadians(Poincare::Preferences::sharedPreferences()->angleUnit());
  return a*sin(b*radianX+c)+d;
}

double TrigonometricModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double radianX = x * toRadians(Poincare::Preferences::sharedPreferences()->angleUnit());
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
