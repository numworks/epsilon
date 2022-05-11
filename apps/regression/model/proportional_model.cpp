#include "proportional_model.h"
#include "../store.h"
#include <poincare/layout_helper.h>
#include <poincare/print.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

Layout ProportionalModel::layout() {
  if (m_layout.isUninitialized()) {
    const char * s = "a·X";
    m_layout = LayoutHelper::StringToCodePointsLayout(s, strlen(s), k_layoutFont);
  }
  return m_layout;
}

int ProportionalModel::buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  return Poincare::Print::safeCustomPrintf(buffer, bufferSize, "%*.*ed·x",
      modelCoefficients[0], displayMode, significantDigits);
}

double ProportionalModel::evaluate(double * modelCoefficients, double x) const {
  return modelCoefficients[0] * x;
}

double ProportionalModel::levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) {
  const double a = modelCoefficients[0];
  if (a == 0.0) {
    return NAN;
  }
  return y/a;
}

double ProportionalModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  assert(derivateCoefficientIndex == 0);
  // Derivate: x
  return x;
}

}
