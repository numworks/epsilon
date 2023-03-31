#include "exponential_model.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/constant.h>
#include <poincare/horizontal_layout.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/print.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

#include "../store.h"

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout ExponentialModel::templateLayout() const {
  Layout base;
  Layout exponent;
  if (m_isAbxForm) {
    base = CodePointLayout::Builder('b'),
    exponent = CodePointLayout::Builder('x');
  } else {
    base = CodePointLayout::Builder('e'),
    exponent = HorizontalLayout::Builder(
        {CodePointLayout::Builder('b'),
         CodePointLayout::Builder(UCodePointMiddleDot),
         CodePointLayout::Builder('x')});
  }
  return HorizontalLayout::Builder(
      {CodePointLayout::Builder('a'),
       CodePointLayout::Builder(UCodePointMiddleDot), base,
       VerticalOffsetLayout::Builder(
           exponent, VerticalOffsetLayoutNode::VerticalPosition::Superscript)});
}

Poincare::Expression ExponentialModel::privateExpression(
    double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // if m_isAbxForm -> a*b^x, else a*e^bx
  return m_isAbxForm ? Multiplication::Builder(
                           Number::DecimalNumber(a),
                           Power::Builder(Number::DecimalNumber(b),
                                          Symbol::Builder(k_xSymbol)))
                     : Multiplication::Builder(
                           Number::DecimalNumber(a),
                           Power::Builder(Constant::ExponentialEBuilder(),
                                          Multiplication::Builder(
                                              Number::DecimalNumber(b),
                                              Symbol::Builder(k_xSymbol))));
}

double ExponentialModel::evaluate(double* modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = aebxFormatBValue(modelCoefficients);
  // a*e^(bx)
  return a * std::exp(b * x);
}

double ExponentialModel::aebxFormatBValue(double* modelCoefficients) const {
  return m_isAbxForm ? std::log(modelCoefficients[1]) : modelCoefficients[1];
}

double ExponentialModel::levelSet(double* modelCoefficients, double xMin,
                                  double xMax, double y,
                                  Poincare::Context* context) {
  double a = modelCoefficients[0];
  double b = aebxFormatBValue(modelCoefficients);
  if (a == 0 || b == 0 || y / a <= 0) {
    return NAN;
  }
  return std::log(y / a) / b;
}

void ExponentialModel::privateFit(Store* store, int series,
                                  double* modelCoefficients,
                                  Poincare::Context* context) {
  assert(store != nullptr && series >= 0 && series < Store::k_numberOfSeries &&
         store->seriesIsActive(series));
  /* By the change of variable z=ln(y), the equation y=a*exp(b*x) becomes
   * z=c*x+d with c=b and d=ln(a). That change of variable does not preserve the
   * default regression error function, so default R2 isn't optimal.
   * However, we compute R2 using that change of variable, so that it matches
   * the fit and is optimal.
   * This change turns this exponential regression problem into a linear one.
   * If the y values are all negative, one may replace each of them by its
   * opposite. */
  bool firstYIsNegative = store->get(series, 1, 0) < 0.0;
  Shared::DoublePairStore::Parameters parameters(false, true, firstYIsNegative);
  modelCoefficients[0] = std::exp(store->yIntercept(series, parameters)) *
                         (firstYIsNegative ? -1.0 : 1.0);
  double slope = store->slope(series, parameters);
  modelCoefficients[1] = m_isAbxForm ? std::exp(slope) : slope;
}

bool ExponentialModel::dataSuitableForFit(Store* store, int series) const {
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

}  // namespace Regression
