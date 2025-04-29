#include "logistic_regression.h"

#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/src/statistics/dataset_adapter.h>

#include <cmath>

namespace Poincare::Internal {

/* NOTE =============
 * Logistic Regression uses a two different equivalent models:
 *
 * The "user" model:     c/(1+a*exp(-bx))      (displayed to the user)
 * The "internal" model: c/(1+exp(-b(x-d)))    (use internally for better fit)
 *
 * We can easily transition from one to the other with a = exp(bd).
 *
 * We use a different model internally because `a` can easily overflow the
 * `double` range when values of x and b where large enough.
 * A typical classroom example is to have a series of years as x. Knowing that
 * `d ~= std::mean(x)` (eg: 2000), this leads to `a ~= exp(2000b)`
 * which reaches infinity for fairly small values of `b`.
 *
 * With the internal model, we can still do the [fit] correctly even when `a`
 * reaches `inf`
 *  */

/* Update coefficients from model to the format displayed to the user.
 * Return [false] if conversion lead to NAN/Inf results */
static bool UpdateToUserCoefficients(
    Regression::Coefficients& modelCoefficients) {
  /* Compute `a` coefficient from "user" model */
  double a = std::exp(modelCoefficients[0] * modelCoefficients[1]);
  modelCoefficients[0] = a;
  return std::isfinite(a);
}

UserExpression LogisticRegression::privateExpression(
    const double* modelCoefficients) const {
  assert(!m_isInternal);
  // User model here: c/(1+a*exp(-b*x))
  return UserExpression::Create(
      KDiv(KC, KAdd(1_e, KMult(KA, KPow(e_e, KOpposite(KMult(KB, "x"_e)))))),
      {.KA = UserExpression::Builder(modelCoefficients[0]),
       .KB = UserExpression::Builder(modelCoefficients[1]),
       .KC = UserExpression::Builder(modelCoefficients[2])});
}

double LogisticRegression::privateEvaluate(
    const Coefficients& modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  if (m_isInternal) {
    double d = a;
    return c / (1.0 + std::exp(-b * (x - d)));
  }
  if (a == 0.0) {
    /* Avoids returning NAN if std::exp(-b * x) == Inf because value is too
     * large. */
    return c;
  }
  return c / (1.0 + a * std::exp(-b * x));
}

Regression::Coefficients LogisticRegression::privateFit(
    const Series* series, Poincare::Context* context) const {
  if (m_isInternal) {
    return Regression::privateFit(series, context);
  }
  Coefficients modelCoefficients;
  Get(Type::LogisticInternal, AngleUnit::Radian)
      ->fit(series, modelCoefficients.data(), context);
  bool success = UpdateToUserCoefficients(modelCoefficients);
  if (!success) {
    // Fallback to all NAN if update fails
    modelCoefficients.fill(NAN);
  }
  return modelCoefficients;
}

double LogisticRegression::levelSet(const double* modelCoefficients,
                                    double xMin, double xMax, double y,
                                    Poincare::Context* context) const {
  assert(!m_isInternal);
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

double LogisticRegression::partialDerivate(
    const Coefficients& modelCoefficients, int derivateCoefficientIndex,
    double x) const {
  assert(m_isInternal);
  double d = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double exp = std::exp(-b * (x - d));
  double denominator = 1.0 + exp;
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to d: -exp(-b*(x-d)) * c * b / (1+exp(-b(x-d)))^2
    return -exp * c * b / (denominator * denominator);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b:
    // (x-d) * exp(-b*(x-d)) * c / (1+exp(-b*(x-d)))^2
    return (x - d) * exp * c / (denominator * denominator);
  }
  assert(derivateCoefficientIndex == 2);
  // Derivate with respect to c: 1/(1+exp(-b*(x-d)))
  return 1.0 / denominator;
}

Regression::Coefficients LogisticRegression::specializedInitCoefficientsForFit(
    double defaultValue, size_t /* attemptNumber */,
    const Series* series) const {
  assert(m_isInternal);
  StatisticsDatasetFromTable xColumn(series, 0);
  StatisticsDatasetFromTable yColumn(series, 1);
  /* We optimize fit for data that actually follow a logistic function curve :
   * f(x)=c/(1+e^(-b(x-d)))
   * We use these properties :
   * (1) f(x) = c/2 <=> x = d
   * (2) f(x + d) = r * c <=> x = ln(r/(1-r)) / b
   * Coefficients are initialized assuming the data is equally distributed on
   * the interesting part of a logistic function curve.
   * We assume this interesting part to be where 0.01*c < f(x) < 0.99*c.
   * Using (2), it roughly corresponds to d-5/b < x < d+5/b
   * Data is assumed equally distributed around the point (d, c/2) (1)
   * The curve and significant values look like this :
   *
   *                                  d
   *                                  ¦  ______  _  _  _  c
   *                                  ¦ /¦
   *             c/2  _  _  _  _  _  _¦/ ¦
   *                                  /  ¦
   *              0  _  _  _  ______ /   ¦
   *                                ¦    ¦
   *                                ¦    ¦
   *                            ~(d-5/b) ¦
   *                                  ~(d+5/b)
   */

  /* We assume the average of Y data is c/2. This handles both positive and
   * negative c coefficients while not being too dependent on outliers. */
  double c = 2.0 * yColumn.mean();

  /* The range of the interesting part is 10/b. We assume that most data points
   * are distributed around the interesting part. We can therefore estimate b
   * from X's range of values (dependent on outliers): Xmax - Xmin = 10/b */
  double b = 10.0 / (xColumn.max() - xColumn.min());
  if (!std::isfinite(b)) {
    b = defaultValue;
  }
  /* The sign of b depends on the sign of c and overall slope of data values :
   * Sign of b if :                  c positive         c negative
   * - Positive slope (__/‾‾) :      b positive         b negative
   * - Negative slope (‾‾\__) :      b negative         b positive */
  if ((series->slope() < 0.0) != (c < 0.0)) {
    b = -b;
  }

  /* We assume the average of X data is d. This handles both positive and
   * negative values while not being too dependent on outliers. */
  double d = xColumn.mean();
  if (!std::isfinite(d)) {
    d = defaultValue;
  }

  return {d, b, c};
}

}  // namespace Poincare::Internal
