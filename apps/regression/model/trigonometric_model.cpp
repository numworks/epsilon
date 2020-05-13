#include "trigonometric_model.h"
#include <apps/regression/store.h>
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

double TrigonometricModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double radianX = x * toRadians(Poincare::Preferences::sharedPreferences()->angleUnit());
  return a*sin(b*radianX+c)+d;
}

double TrigonometricModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 3) {
    // Derivate with respect to d: 1
    return 1.0;
  }

  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double radianX = x * toRadians(Poincare::Preferences::sharedPreferences()->angleUnit());

  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: sin(b*x+c)
    return sin(b * radianX + c);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b: x*a*cos(b*x+c);
    return radianX * a * cos(b * radianX + c);
  }
  assert(derivateCoefficientIndex == 2);
  // Derivatewith respect to c: a*cos(b*x+c)
  return a * cos(b * radianX + c);
}

void TrigonometricModel::specializedInitCoefficientsForFit(double * modelCoefficients, double defaultValue, Store * store, int series) const {
  assert(store != nullptr && series >= 0 && series < Store::k_numberOfSeries && !store->seriesIsEmpty(series));
  for (int i = 1; i < k_numberOfCoefficients - 1; i++) {
    modelCoefficients[i] = defaultValue;
  }
  /* We try a better initialization than the default value. We hope that this
   * will improve the gradient descent to find correct coefficients.
   *
   * Init the "amplitude" coefficient. We take twice the standard deviation,
   * because for a normal law, this interval contains 99.73% of the values. We
   * do not take half of the apmlitude of the series, because this would be too
   * dependant on outliers. */
  modelCoefficients[0] = 3.0*store->standardDeviationOfColumn(series, 1);
  // Init the "y delta" coefficient
  modelCoefficients[k_numberOfCoefficients - 1] = store->meanOfColumn(series, 1);
}

Expression TrigonometricModel::expression(double * modelCoefficients) {
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
  return result;
}

}
