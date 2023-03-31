#include "power_model.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/print.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

#include "../store.h"

using namespace Poincare;

namespace Regression {

Layout PowerModel::templateLayout() const {
  return HorizontalLayout::Builder(
      {CodePointLayout::Builder('a'),
       CodePointLayout::Builder(UCodePointMiddleDot),
       CodePointLayout::Builder('x'),
       VerticalOffsetLayout::Builder(
           CodePointLayout::Builder('b'),
           VerticalOffsetLayoutNode::VerticalPosition::Superscript)});
}

Expression PowerModel::privateExpression(double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // a*x^b
  return Multiplication::Builder(
      Number::DecimalNumber(a),
      Power::Builder(Symbol::Builder(k_xSymbol), Number::DecimalNumber(b)));
}

double PowerModel::evaluate(double* modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a * std::pow(x, b);
}

double PowerModel::levelSet(double* modelCoefficients, double xMin, double xMax,
                            double y, Poincare::Context* context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (a == 0 || b == 0 || y / a <= 0) {
    return NAN;
  }
  return std::exp(std::log(y / a) / b);
}

void PowerModel::privateFit(Store* store, int series, double* modelCoefficients,
                            Poincare::Context* context) {
  /* Y1 = aX1^b => ln(Y1) = ln(a) + b*ln(X1)*/
  Shared::DoublePairStore::Parameters parameters(true, true, false);
  modelCoefficients[0] = std::exp(store->yIntercept(series, parameters));
  modelCoefficients[1] = store->slope(series, parameters);
}

bool PowerModel::dataSuitableForFit(Store* store, int series) const {
  if (!Model::dataSuitableForFit(store, series)) {
    return false;
  }
  int numberOfPairs = store->numberOfPairsOfSeries(series);
  for (int j = 0; j < numberOfPairs; j++) {
    if (store->get(series, 0, j) < 0) {
      return false;
    }
  }
  return true;
}

}  // namespace Regression
