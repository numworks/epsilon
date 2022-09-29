#include "affine_model.h"
#include "../store.h"
#include <poincare/layout_helper.h>
#include <poincare/print.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

Layout AffineModel::layout() {
  if (m_layout.isUninitialized()) {
    m_layout = LayoutHelper::StringToCodePointsLayout(layoutString(), strlen(layoutString()));
  }
  return m_layout;
}

int AffineModel::buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  return Poincare::Print::SafeCustomPrintf(buffer, bufferSize, equationTemplate(),
      modelCoefficients[0], displayMode, significantDigits,
      modelCoefficients[1], displayMode, significantDigits);
}

double AffineModel::evaluate(double * modelCoefficients, double x) const {
  double slope = modelCoefficients[slopeCoefficientIndex()];
  double yIntercept = modelCoefficients[yInterceptCoefficientIndex()];
  return slope * x + yIntercept;
}

double AffineModel::levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) {
  double slope = modelCoefficients[slopeCoefficientIndex()];
  double yIntercept = modelCoefficients[yInterceptCoefficientIndex()];
  if (slope == 0) {
    return NAN;
  }
  return (y - yIntercept) / slope;
}

double AffineModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == slopeCoefficientIndex()) {
    // Derivate with respect to slope: x
    return x;
  }
  assert(derivateCoefficientIndex == yInterceptCoefficientIndex());
  // Derivate with respect to yIntercept: 1
  return 1.0;
}

}
