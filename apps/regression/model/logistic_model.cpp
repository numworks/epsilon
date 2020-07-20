#include "logistic_model.h"
#include "../store.h"
#include <math.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Regression {

Layout LogisticModel::layout() {
  if (m_layout.isUninitialized()) {
    m_layout = FractionLayout::Builder(
      CodePointLayout::Builder('c', k_layoutFont),
      HorizontalLayout::Builder({
        CodePointLayout::Builder('1', k_layoutFont),
        CodePointLayout::Builder('+', k_layoutFont),
        CodePointLayout::Builder('a', k_layoutFont),
        CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
        CodePointLayout::Builder('e', k_layoutFont),
        VerticalOffsetLayout::Builder(
          HorizontalLayout::Builder({
            CodePointLayout::Builder('-', k_layoutFont),
            CodePointLayout::Builder('b', k_layoutFont),
            CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
            CodePointLayout::Builder('X', k_layoutFont)
          }),
          VerticalOffsetLayoutNode::Position::Superscript
        )
      })
    );
  }
  return m_layout;
}

double LogisticModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return c/(1.0+a*exp(-b*x));
}

double LogisticModel::levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  if (a == 0 || b == 0 || c == 0 || y == 0) {
    return NAN;
  }
  double lnArgument = (c/y - 1)/a;
  if (lnArgument <= 0) {
    return NAN;
  }
  return -log(lnArgument)/b;
}

double LogisticModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double denominator = 1.0 + a * exp(-b * x);
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: exp(-b*x)*(-1 * c/(1.0+a*exp(-b*x))^2)
    return -exp(-b * x) * c / (denominator * denominator);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b: (-x)*a*exp(-b*x)*(-1/(1.0+a*exp(-b*x))^2)
    return x * a * exp(-b * x) * c / (denominator * denominator);
  }
  assert(derivateCoefficientIndex == 2);
  // Derivate with respect to c: (-x)*a*exp(-b*x)*(-1/(1.0+a*exp(-b*x))^2)
  return 1.0 / denominator;
}

void LogisticModel::specializedInitCoefficientsForFit(double * modelCoefficients, double defaultValue, Store * store, int series) const {
  assert(store != nullptr && series >= 0 && series < Store::k_numberOfSeries && !store->seriesIsEmpty(series));
  modelCoefficients[0] = defaultValue;
  modelCoefficients[1] = defaultValue;
  /* If the data is a standard logistic function, the ordinates are between 0
   * and c. Twice the standard vertical deviation is a rough estimate of c
   * that is "close enough" to c to seed the coefficient, without being too
   * dependent on outliers.*/
  modelCoefficients[2] = 2.0 * store->standardDeviationOfColumn(series, 1);
}


}
