#include "logarithmic_model.h"
#include "../store.h"
#include <math.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>

using namespace Poincare;

namespace Regression {

Layout LogarithmicModel::layout() {
  if (m_layout.isUninitialized()) {
    Layout layoutChildren[] = {
      CodePointLayout::Builder('a', KDFont::SmallFont),
      CodePointLayout::Builder(UCodePointMiddleDot, KDFont::SmallFont),
      CodePointLayout::Builder('l', KDFont::SmallFont),
      CodePointLayout::Builder('n', KDFont::SmallFont),
      CodePointLayout::Builder('(', KDFont::SmallFont),
      CodePointLayout::Builder('X', KDFont::SmallFont),
      CodePointLayout::Builder(')', KDFont::SmallFont),
      CodePointLayout::Builder('+', KDFont::SmallFont),
      CodePointLayout::Builder('b', KDFont::SmallFont)
    };
    m_layout = HorizontalLayout::Builder(layoutChildren, 9);
  }
  return m_layout;
}

double LogarithmicModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a*log(x)+b;
}

double LogarithmicModel::levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (a == 0) {
    return NAN;
  }
  return exp((y-b)/a);
}

double LogarithmicModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate: ln(x)
    assert(x >0);
    return log(x);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: 1
    return 1;
  }
  assert(false);
  return 0.0;
}

bool LogarithmicModel::dataSuitableForFit(Store * store, int series) const {
  if (!Model::dataSuitableForFit(store, series)) {
    return false;
  }
  int numberOfPairs = store->numberOfPairsOfSeries(series);
  for (int j = 0; j < numberOfPairs; j++) {
    if (store->get(series, 0, j) <= 0) {
      return false;
    }
  }
  return true;
}

}
