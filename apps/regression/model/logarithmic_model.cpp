#include "logarithmic_model.h"
#include "../store.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

LayoutReference LogarithmicModel::layout() {
  static LayoutReference layout;
  if (layout.isUninitialized()) {
    const LayoutReference layoutChildren[] = {
      CharLayoutRef('a', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('l', KDText::FontSize::Small),
      CharLayoutRef('n', KDText::FontSize::Small),
      CharLayoutRef('(', KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      CharLayoutRef(')', KDText::FontSize::Small),
      CharLayoutRef('+', KDText::FontSize::Small),
      CharLayoutRef('b', KDText::FontSize::Small)
    };
    layout = HorizontalLayoutRef(layoutChildren, 9);
  }
  return layout;
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
