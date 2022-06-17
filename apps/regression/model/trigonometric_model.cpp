#include "trigonometric_model.h"
#include <apps/regression/store.h>
#include "../../shared/poincare_helpers.h"
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/number.h>
#include <poincare/power.h>
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <poincare/sine.h>
#include <poincare/symbol.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Regression {

static double toRadians() {
  return M_PI / Trigonometry::PiInAngleUnit(Poincare::Preferences::sharedPreferences()->angleUnit());
}

Layout TrigonometricModel::layout() {
  if (m_layout.isUninitialized()) {
    const char * s = "a·sin(b·x+c)+d";
    m_layout = LayoutHelper::StringToCodePointsLayout(s, strlen(s), k_layoutFont);
  }
  return m_layout;
}

int TrigonometricModel::buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  return Poincare::Print::safeCustomPrintf(buffer, bufferSize, "%*.*ed·sin(%*.*ed·x%+*.*ed)%+*.*ed",
      modelCoefficients[0], displayMode, significantDigits,
      modelCoefficients[1], displayMode, significantDigits,
      modelCoefficients[2], displayMode, significantDigits,
      modelCoefficients[3], displayMode, significantDigits);
}

double TrigonometricModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double radian = toRadians();
  // sin() is here defined for radians, so b*x+c are converted in radians.
  return a * std::sin(radian * (b * x + c)) + d;
}

double TrigonometricModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 3) {
    // Derivate with respect to d: 1
    return 1.0;
  }

  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double radian = toRadians();
  /* sin() and cos() are here defined for radians, so b*x+c are converted in
   * radians. The added coefficient also appear in derivatives. */
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: sin(b*x+c)
    return std::sin(radian * (b * x + c));
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b: x*a*cos(b*x+c);
    return radian * x * a * std::cos(radian * (b * x + c));
  }
  assert(derivateCoefficientIndex == 2);
  // Derivate with respect to c: a*cos(b*x+c)
  return radian * a * std::cos(radian * (b * x + c));
}

void TrigonometricModel::specializedInitCoefficientsForFit(double * modelCoefficients, double defaultValue, Store * store, int series) const {
  assert(store != nullptr && series >= 0 && series < Store::k_numberOfSeries && store->seriesIsValid(series));
  /* We try a better initialization than the default value. We hope that this
   * will improve the gradient descent to find correct coefficients.
   *
   * Init the "amplitude" coefficient. We take twice the standard deviation,
   * because for a normal law, this interval contains 99.73% of the values. We
   * do not take half of the amplitude of the series, because this would be too
   * dependent on outliers. */
  modelCoefficients[0] = 3.0*store->standardDeviationOfColumn(series, 1);
  // Init the "y delta" coefficient
  modelCoefficients[k_numberOfCoefficients - 1] = store->meanOfColumn(series, 1);
  // Init the b coefficient
  double rangeX = store->maxValueOfColumn(series, 0) - store->minValueOfColumn(series, 0);
  double piInAngleUnit = Trigonometry::PiInAngleUnit(Poincare::Preferences::sharedPreferences()->angleUnit());
  if (rangeX > 0) {
    /* b/2π represents the frequency of the sine (in radians). Instead of
     * initializing it to 0, we use the inverse of X series' range as an order
     * of magnitude for it. It can help avoiding a regression that overfits the
     * data with a very high frequency. This period also depends on the
     * angleUnit. We take it into account so that it doesn't impact the result
     * (although coefficients b and c depends on the angleUnit). */
    modelCoefficients[1] = (2.0 * piInAngleUnit) / rangeX;
  } else {
    // Coefficient b must not depend on angleUnit.
    modelCoefficients[1] = defaultValue * piInAngleUnit;
  }
  /* No shift is assumed, coefficient c is set to 0.
   * If it were to be non-null, angleUnit must be taken into account.
   * modelCoefficients[2] = initialCValue * piInAngleUnit; */
  modelCoefficients[2] = 0.0;
}

void TrigonometricModel::uniformizeCoefficientsFromFit(double * modelCoefficients) const {
  // Coefficients must be unique.
  double piInAngleUnit = Trigonometry::PiInAngleUnit(Poincare::Preferences::sharedPreferences()->angleUnit());
  // A must be positive.
  if (modelCoefficients[0] < 0.0) {
    // A * sin(B * x + C) + D = -A * sin(B * x + C + π) + D
    modelCoefficients[0] *= -1.0;
    modelCoefficients[2] += piInAngleUnit;
  }
  // B must be positive.
  if (modelCoefficients[1] < 0.0) {
    /* A * sin(B * x + C) + D = -A * sin(-B * x - C) + D
     * -A * sin(-B * x - C) + D = A * sin(-B * x - C + π) + D */
    modelCoefficients[1] *= -1.0;
    modelCoefficients[2] *= -1.0;
    modelCoefficients[2] += piInAngleUnit;
  }
  // C must be between -π (excluded) and π (included).
  if (modelCoefficients[2] <= -piInAngleUnit || modelCoefficients[2] > piInAngleUnit) {
    /* A*sin(B*x + C) + D = A*sin(B*x + C - 2π) = A*sin(B*x + C + 2π)
     * Using remainder(C,2π) = C - 2π * round(C / 2π) */
    modelCoefficients[2] -= 2.0 * piInAngleUnit * std::round(modelCoefficients[2] / (2.0 * piInAngleUnit));
    if (modelCoefficients[2] == -piInAngleUnit) {
      // Keep π instead of -π
      modelCoefficients[2] = piInAngleUnit;
    }
  }
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
