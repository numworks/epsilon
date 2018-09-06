#include "power_model.h"
#include "../store.h"
#include <math.h>
#include <assert.h>
#include "../../poincare/include/poincare_layouts.h"

using namespace Poincare;

namespace Regression {

ExpressionLayout * PowerModel::layout() {
  static ExpressionLayout * layout = nullptr;
  if (layout == nullptr) {
    const ExpressionLayout * layoutChildren[] = {
      CharLayoutRef('a', KDText::FontSize::Small),
      CharLayoutRef(Ion::Charset::MiddleDot, KDText::FontSize::Small),
      CharLayoutRef('X', KDText::FontSize::Small),
      VerticalOffsetLayoutRef(
          CharLayoutRef('b', KDText::FontSize::Small),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      };
    layout = HorizontalLayoutRef(layoutChildren, 4);
  }
  return layout;
}

double PowerModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a*pow(x,b);
}

double PowerModel::levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (a == 0 || b == 0|| y/a <= 0) {
    return NAN;
  }
  return exp(log(y/a)/b);
}

double PowerModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (derivateCoefficientIndex == 0) {
    // Derivate: pow(x,b)
    return pow(x,b);
  }
  if (derivateCoefficientIndex == 1) {
    assert(x >= 0);
    /* We assume all xi are positive.
     * For x = 0, a*pow(x,b) = 0, the partial derivate along b is 0
     * For x > 0, a*pow(x,b) = a*exp(b*ln(x)), the partial derivate along b is
     *   ln(x)*a*pow(x,b) */
    return x == 0 ? 0 : log(x)*a*pow(x, b);
  }
  assert(false);
  return 0.0;
}

bool PowerModel::dataSuitableForFit(Store * store, int series) const {
  if (!Model::dataSuitableForFit(store, series)) {
    return false;
  }
  int numberOfPairs = store->numberOfPairsOfSeries(series);
  for (int j = 0; j < numberOfPairs; j++) {
    if (store->get(series, 0, j) < 0) {
      return false;
    }
  }
  return true;
}


}
