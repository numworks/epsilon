#include "power_model.h"
#include "../store.h"
#include <math.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Regression {

Layout PowerModel::layout() {
  if (m_layout.isUninitialized()) {
    m_layout = HorizontalLayout::Builder({
      CodePointLayout::Builder('a', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('X', k_layoutFont),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('b', k_layoutFont),
        VerticalOffsetLayoutNode::Position::Superscript
      ),
    });
  }
  return m_layout;
}

double PowerModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a*pow(x,b);
}

double PowerModel::levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (a == 0 || b == 0|| y/a <= 0) {
    return NAN;
  }
  return exp(log(y/a)/b);
}

double PowerModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: pow(x,b)
    return pow(x,b);
  }
  assert(derivateCoefficientIndex == 1);
  assert(x >= 0);
  /* We assume all xi are positive.
   * For x = 0, a*pow(x,b) = 0, the partial derivate with respect to b is 0
   * For x > 0, a*pow(x,b) = a*exp(b*ln(x)), the partial derivate with respect
   *            to b is ln(x)*a*pow(x,b) */
  return x == 0.0 ? 0.0 : log(x) * a * pow(x, b);
}

void PowerModel::fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) {
  /* Y1 = aX1^b => ln(Y1) = ln(a) + b*ln(X1)*/
  modelCoefficients[0] = exp(store->yIntercept(series, true));
  modelCoefficients[1] = store->slope(series, true);
}

bool PowerModel::dataSuitableForFit(Store * store, int series) const {
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


}
