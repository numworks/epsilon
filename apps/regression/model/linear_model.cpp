#include "linear_model.h"
#include "../store.h"
#include <math.h>
#include <poincare/layout_engine.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

ExpressionLayout * LinearModel::layout() {
  static ExpressionLayout * layout = LayoutEngine::createStringLayout("aX+b", 4, KDText::FontSize::Small);
  return layout;
}

double LinearModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a*x+b;
}

double LinearModel::levelSet(double * modelCoefficients, double y) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (a == 0) {
    return NAN;
  }
  return (y-b)/a;
}

void LinearModel::fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) {
  modelCoefficients[0] = store->slope(series);
  modelCoefficients[1] = store->yIntercept(series);
}

double LinearModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate: x
    return x;
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: 1;
    return 1;
  }
  assert(false);
  return 0.0;
}

}
