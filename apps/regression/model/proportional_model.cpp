#include "proportional_model.h"
#include "../store.h"
#include <poincare/layout_helper.h>
#include <math.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

Layout ProportionalModel::layout() {
  if (m_layout.isUninitialized()) {
    const char * s = "a·X";
    m_layout = LayoutHelper::String(s, strlen(s), k_layoutFont);
  }
  return m_layout;
}

double ProportionalModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  return a*x;
}

double ProportionalModel::levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  if (a == 0.0) {
    return NAN;
  }
  return y/a;
}

void ProportionalModel::fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) {
  modelCoefficients[0] = store->slope(series);
}

double ProportionalModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate: x
    return x;
  }
  assert(false);
  return 0.0;
}

}
