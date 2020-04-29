#include "affine_model.h"
#include "../store.h"
#include <poincare/layout_helper.h>
#include <math.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

Layout AffineModel::layout() {
  if (m_layout.isUninitialized()) {
    const char * s = "a·X";
    m_layout = LayoutHelper::String(s, strlen(s), k_layoutFont);
  }
  return m_layout;
}

double AffineModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  return a*x;
}

double AffineModel::levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (a == 0) {
    return NAN;
  }
  return y-b;
}

void AffineModel::fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) {
  modelCoefficients[0] = store->slope(series);
  modelCoefficients[1] = store->yIntercept(series);
}

double AffineModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
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
