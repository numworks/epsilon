#include "logistic_regression.h"

#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include <cmath>

#include "dataset_adapter.h"

namespace Poincare::Regression {

Poincare::Layout LogisticRegression::templateLayout() const {
  return KRackL(KFracL("c"_l, "1+a·e"_l ^ KSuperscriptL("-b·x"_l)));
}

Poincare::UserExpression LogisticRegression::privateExpression(
    double* modelCoefficients) const {
  // c/(1+a*e^(-b*x))
  return Poincare::NewExpression::Create(
      KDiv(KC, KAdd(1_e, KMult(KA, KExp(KMult(-1_e, KB, "x"_e))))),
      {.KA = Poincare::NewExpression::Builder<double>(modelCoefficients[0]),
       .KB = Poincare::NewExpression::Builder<double>(modelCoefficients[1]),
       .KC = Poincare::NewExpression::Builder<double>(modelCoefficients[2])});
}

double LogisticRegression::evaluate(double* modelCoefficients, double x) const {
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

double LogisticRegression::levelSet(double* modelCoefficients, double xMin,
                                    double xMax, double y,
                                    Poincare::Context* context) const {
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

double LogisticRegression::partialDerivate(double* modelCoefficients,
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

void LogisticRegression::specializedInitCoefficientsForFit(
    double* modelCoefficients, double defaultValue,
    const Series* series) const {
  OneColumn xColumn(series, 0);
  OneColumn yColumn(series, 1);
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
  double c = 2.0 * yColumn.mean();

  /* The range of the interesting part is 10/b. We assume that most data points
   * are distributed around the interesting part. We can therefore estimate b
   * from X's range of values (dependent on outliers): Xmax - Xmin = 10/b */
  double b = 10.0 / (xColumn.max() - xColumn.min());
  double slope = DatasetSeriesAdapter(series).slope();
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
  double a = std::exp(b * xColumn.mean());
  if (!std::isfinite(a)) {
    a = defaultValue;
  }

  modelCoefficients[0] = a;
  modelCoefficients[1] = b;
  modelCoefficients[2] = c;
}

}  // namespace Poincare::Regression
