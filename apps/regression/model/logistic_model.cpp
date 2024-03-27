#include "logistic_model.h"

#include <assert.h>
#include <poincare/based_integer.h>
#include <poincare/constant.h>
#include <poincare/division.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/print.h>

#include <cmath>

#include "../store.h"

using namespace Poincare;

namespace Regression {

using namespace PoincareJ;

Layout LogisticModel::templateLayout() const {
  return KRackL(KFracL("c"_l, "1+a·e"_l ^ KSuperscriptL("-b·x"_l)));
}

Poincare::Expression LogisticModel::privateExpression(
    double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  // c/(1+a*e^(-b*x))
  return Division::Builder(
      Number::DecimalNumber(c),
      AdditionOrSubtractionBuilder(
          BasedInteger::Builder(1),
          Multiplication::Builder(
              Number::DecimalNumber(std::fabs(a)),
              Power::Builder(
                  Constant::ExponentialEBuilder(),
                  Multiplication::Builder(Number::DecimalNumber(-b),
                                          Symbol::Builder(k_xSymbol)))),
          a >= 0.0));
}

double LogisticModel::evaluate(double* modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  if (a == 0.0) {
    /* Avoids returning NAN if std::exp(-b * x) == Inf because value is too
     * large. */
    return c;
  }
  return c / (1.0 + a * std::exp(-b * x));
}

double LogisticModel::levelSet(double* modelCoefficients, double xMin,
                               double xMax, double y,
                               Poincare::Context* context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  if (a == 0 || b == 0 || c == 0 || y == 0) {
    return NAN;
  }
  double lnArgument = (c / y - 1) / a;
  if (lnArgument <= 0) {
    return NAN;
  }
  return -std::log(lnArgument) / b;
}

double LogisticModel::partialDerivate(double* modelCoefficients,
                                      int derivateCoefficientIndex,
                                      double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double denominator = 1.0 + a * std::exp(-b * x);
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: exp(-b*x)*(-c/(1+a*exp(-b*x))^2)
    return -std::exp(-b * x) * c / (denominator * denominator);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b: (-x)*a*exp(-b*x)*(-c/(1+a*exp(-b*x))^2)
    return x * a * std::exp(-b * x) * c / (denominator * denominator);
  }
  assert(derivateCoefficientIndex == 2);
  // Derivate with respect to c: 1/(1+a*exp(-b*x))
  return 1.0 / denominator;
}

void LogisticModel::specializedInitCoefficientsForFit(double* modelCoefficients,
                                                      double defaultValue,
                                                      Store* store,
                                                      int series) const {
  assert(store != nullptr && series >= 0 && series < Store::k_numberOfSeries &&
         store->seriesIsActive(series));
  /* We optimize fit for data that actually follow a logistic function curve :
   * f(x)=c/(1+a*e^(-bx))
   * We use these properties :
   * (1) f(x) = c/2 <=> x = ln(a)/b
   * (2) f(x + ln(a)/b) = r * c <=> x = ln(r/(1-r)) / b
   * Coefficients are initialized assuming the data is equally distributed on
   * the interesting part of a logistic function curve.
   * We assume this interesting part to be where 0.01*c < f(x) < 0.99*c.
   * Using (2), it roughly corresponds to (ln(a)-5)/b < x < (ln(a)+5)/b
   * Data is assumed equally distributed around the point (ln(a)/b, c/2) (1)
   * The curve and significant values look like this :
   *
   *                             ln(a)/b
   *                                  ¦  ______  _  _  _  c
   *                                  ¦ /¦
   *             c/2  _  _  _  _  _  _¦/ ¦
   *                                  /  ¦
   *              0  _  _  _  ______ /   ¦
   *                                ¦    ¦
   *                                ¦    ¦
   *                      ~(ln(a)-5)/b   ~(ln(a)+5)/b
   */

  /* We assume the average of Y data is c/2. This handles both positive and
   * negative c coefficients while not being too dependent on outliers. */
  double c = 2.0 * store->meanOfColumn(series, 1);

  /* The range of the interesting part is 10/b. We assume that most data points
   * are distributed around the interesting part. We can therefore estimate b
   * from X's range of values (dependent on outliers): Xmax - Xmin = 10/b */
  double b = 10.0 / (store->maxValueOfColumn(series, 0) -
                     store->minValueOfColumn(series, 0));
  double slope = store->slope(series);
  if (!std::isfinite(b)) {
    b = defaultValue;
  }
  /* The sign of b depends on the sign of c and overall slope of data values :
   * Sign of b if :                  c positive         c negative
   * - Positive slope (__/‾‾) :      b positive         b negative
   * - Negative slope (‾‾\__) :      b negative         b positive */
  if ((slope < 0.0) != (c < 0.0)) {
    b = -b;
  }

  /* We assume the average of X data is ln(a)/b. This handles both positive and
   * negative values while not being too dependent on outliers. */
  double a = std::exp(b * store->meanOfColumn(series, 0));
  if (!std::isfinite(a)) {
    a = defaultValue;
  }

  modelCoefficients[0] = a;
  modelCoefficients[1] = b;
  modelCoefficients[2] = c;
}

}  // namespace Regression
