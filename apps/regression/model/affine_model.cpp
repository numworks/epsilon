#include "affine_model.h"
#include "../store.h"
#include <poincare/layout_helper.h>
#include <math.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

Layout AffineModel::layout() {
  if (m_layout.isUninitialized()) {
    static constexpr const char * s = "a·X+b";
    m_layout = LayoutHelper::StringToCodePointsLayout(s, strlen(s), k_layoutFont);
  }
  return m_layout;
}

double AffineModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a*x+b;
}

double AffineModel::levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (a == 0) {
    return NAN;
  }
  return (y-b)/a;
}

double AffineModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: x
    return x;
  }
  assert(derivateCoefficientIndex == 1);
  // Derivate with respect to b: 1
  return 1.0;
}

}

