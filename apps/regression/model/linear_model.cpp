#include "linear_model.h"
#include "../store.h"
#include "../../poincare/include/poincare_layouts.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

ExpressionLayout * LinearModel::layout() {
  static ExpressionLayout * layout = nullptr;
  if (layout == nullptr) {
    const ExpressionLayout * layoutChildren[] = {
      new CharLayout('a', KDText::FontSize::Small),
      new CharLayout(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      new CharLayout('X', KDText::FontSize::Small),
      new CharLayout('+', KDText::FontSize::Small),
      new CharLayout('b', KDText::FontSize::Small),
    };
    layout = new HorizontalLayout(layoutChildren, 5, false);
  }
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
