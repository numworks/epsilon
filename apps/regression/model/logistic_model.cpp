#include "logistic_model.h"
#include "../store.h"
#include <cmath>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/print.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Regression {

Layout LogisticModel::layout() {
  if (m_layout.isUninitialized()) {
    m_layout = FractionLayout::Builder(
      CodePointLayout::Builder('c', k_layoutFont),
      HorizontalLayout::Builder({
        CodePointLayout::Builder('1', k_layoutFont),
        CodePointLayout::Builder('+', k_layoutFont),
        CodePointLayout::Builder('a', k_layoutFont),
        CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
        CodePointLayout::Builder('e', k_layoutFont),
        VerticalOffsetLayout::Builder(
          HorizontalLayout::Builder({
            CodePointLayout::Builder('-', k_layoutFont),
            CodePointLayout::Builder('b', k_layoutFont),
            CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
            CodePointLayout::Builder('x', k_layoutFont)
          }),
          VerticalOffsetLayoutNode::Position::Superscript
        )
      })
    );
  }
  return m_layout;
}

int LogisticModel::buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  return Poincare::Print::safeCustomPrintf(buffer, bufferSize, "%*.*ed/(1%+*.*ed·e^(%*.*ed·x))",
      modelCoefficients[2], displayMode, significantDigits,
      modelCoefficients[0], displayMode, significantDigits,
      -modelCoefficients[1], displayMode, significantDigits);
}

double LogisticModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return c/(1.0+a*std::exp(-b*x));
}

double LogisticModel::levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  if (a == 0 || b == 0 || c == 0 || y == 0) {
    return NAN;
  }
  double lnArgument = (c/y - 1)/a;
  if (lnArgument <= 0) {
    return NAN;
  }
  return -std::log(lnArgument)/b;
}

double LogisticModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double denominator = 1.0 + a * std::exp(-b * x);
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: exp(-b*x)*(-1 * c/(1.0+a*exp(-b*x))^2)
    return -std::exp(-b * x) * c / (denominator * denominator);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b: (-x)*a*exp(-b*x)*(-1/(1.0+a*exp(-b*x))^2)
    return x * a * std::exp(-b * x) * c / (denominator * denominator);
  }
  assert(derivateCoefficientIndex == 2);
  // Derivate with respect to c: (-x)*a*exp(-b*x)*(-1/(1.0+a*exp(-b*x))^2)
  return 1.0 / denominator;
}

void LogisticModel::specializedInitCoefficientsForFit(double * modelCoefficients, double defaultValue, Store * store, int series) const {
  assert(store != nullptr && series >= 0 && series < Store::k_numberOfSeries && store->seriesIsValid(series));
  /* We optimize fit for data that actually follow a logistic function curve :
   * f(x)=c/(1+a*e^(-bx))
   * We use these properties :
   * (1) f(x) = c/2 <=> x = ln(a)/b
   * (2) f(x + ln(a)/b) = r * c <=> x = ln(r/(1-r)) / b
   * Coefficients are initiated assuming the data is equally distributed on the
   * interesting part of a logistic function curve.
   * We assume this interesting part to be where 0.01*c < f(x) < 0.99*c.
   * Using (2), it roughly corresponds to (ln(a)-5)/b < x < (ln(a)+5)/b
   * Data is assumed equally distributed around the point (ln(a)/b, c/2) (1)
   * The curve and significant values look like this :
   *                             ln(a)/b         _  _  _ c
   *                   c/2  _  _  _  _ ¦/¦‾‾‾‾‾‾
   *                             ______/ ¦
   *                   0 ‾  ‾  ‾        ~(ln(a)+5)/b
   */

  /* We assume the average of Y data is c/2. This handles both positive and
   * negative c coefficients while not being too dependent on outliers. */
  double c = 2.0 * store->meanOfColumn(series, 1);

  /* We assume most data point are distributed around the interesting part,
   * where X data is within (ln(a)-5)/b  and (ln(a)+5)/b (2). We can therefore
   * estimate b from X's range of values (dependent on outliers). */
  double b = 10.0 / (store->maxValueOfColumn(series, 0) - store->minValueOfColumn(series, 0));
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


}
