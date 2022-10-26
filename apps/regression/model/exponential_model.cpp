#include "exponential_model.h"
#include "../store.h"
#include "../../shared/linear_regression_store.h"
#include <cmath>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/print.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout ExponentialModel::layout() {
  if (m_layout.isUninitialized()) {
    Layout base;
    Layout exponent;
    if (m_isAbxForm) {
      base = CodePointLayout::Builder('b'),
      exponent = CodePointLayout::Builder('x');
    } else {
      base = CodePointLayout::Builder('e'),
      exponent = HorizontalLayout::Builder({
        CodePointLayout::Builder('b'),
        CodePointLayout::Builder(UCodePointMiddleDot),
        CodePointLayout::Builder('x')
        });
    }
    m_layout = HorizontalLayout::Builder({
      CodePointLayout::Builder('a'),
      CodePointLayout::Builder(UCodePointMiddleDot),
      base,
      VerticalOffsetLayout::Builder(
        exponent,
        VerticalOffsetLayoutNode::VerticalPosition::Superscript
      )
    });
  }
  return m_layout;
}

int ExponentialModel::buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  return Poincare::Print::SafeCustomPrintf(buffer, bufferSize, m_isAbxForm ? "%*.*ed·%*.*ed^x" : "%*.*ed·e^%*.*ed·x",
      modelCoefficients[0], displayMode, significantDigits,
      modelCoefficients[1], displayMode, significantDigits);
}

double ExponentialModel::aebxFormatBValue(double * modelCoefficients) const {
  return m_isAbxForm ? std::log(modelCoefficients[1]) : modelCoefficients[1];
}

double ExponentialModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = aebxFormatBValue(modelCoefficients);
  // a*e^(bx)
  return a*std::exp(b*x);
}

double ExponentialModel::levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = aebxFormatBValue(modelCoefficients);
  if (a == 0 || b == 0 || y/a <= 0) {
    return NAN;
  }
  return std::log(y/a)/b;
}

void ExponentialModel::privateFit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) {
  assert(store != nullptr && series >= 0 && series < Store::k_numberOfSeries && store->seriesIsValid(series));
  /* By the change of variable z=ln(y), the equation y=a*exp(b*x) becomes
   * z=c*x+d with c=b and d=ln(a). That change of variable does not preserve the
   * default regression error function, so default R2 isn't optimal.
   * However, we compute R2 using that change of variable, so that it matches
   * the fit and is optimal.
   * This change turns this exponential regression problem into a linear one.
   * If the y values are all negative, one may replace each of them by its
   * opposite. */
  double sumOfX = 0;
  double sumOfY = 0;
  double sumOfXX = 0;
  double sumOfXY = 0;
  const int numberOfPoints = store->numberOfPairsOfSeries(series);
  const int sign = store->get(series, 1, 0) > 0 ? 1 : -1;
  for (int p = 0; p < numberOfPoints; p++) {
    const double x = store->get(series, 0, p);
    const double z = store->get(series, 1, p) * sign;
    assert(z > 0.0);
    const double y = std::log(z);
    sumOfX += x;
    sumOfY += y;
    sumOfXX += x*x;
    sumOfXY += x*y;
  }
  const double meanOfX = sumOfX / numberOfPoints;
  const double meanOfY = sumOfY / numberOfPoints;
  const double meanOfXX = sumOfXX / numberOfPoints;
  const double meanOfXY = sumOfXY / numberOfPoints;
  const double variance = meanOfXX - meanOfX * meanOfX;
  const double covariance = meanOfXY - meanOfX * meanOfY;
  const double slope = LinearModelHelper::Slope(covariance, variance);
  modelCoefficients[1] = m_isAbxForm ? std::exp(slope) : slope;
  modelCoefficients[0] =
    sign * std::exp(LinearModelHelper::YIntercept(meanOfY, meanOfX, slope));
}

double ExponentialModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  const double b = aebxFormatBValue(modelCoefficients);
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: exp(b*x)
    return std::exp(b*x);
  }
  assert(derivateCoefficientIndex == 1);
  // Derivate with respect to b: a*x*exp(b*x)
  double a = modelCoefficients[0];
  return a*x*std::exp(b*x);
}

bool ExponentialModel::dataSuitableForFit(Store * store, int series) const {
  if (!Model::dataSuitableForFit(store, series)) {
    return false;
  }
  // All Y data must be of the same sign and non-null
  int numberOfPairs = store->numberOfPairsOfSeries(series);
  assert(numberOfPairs > 0);
  bool coefficientsAreNegative = store->get(series, 1, 0) < 0.0;
  for (int j = 0; j < numberOfPairs; j++) {
    double value = store->get(series, 1, j);
    if (value == 0.0 || coefficientsAreNegative != (value < 0.0)) {
      return false;
    }
  }
  return true;
}

}
