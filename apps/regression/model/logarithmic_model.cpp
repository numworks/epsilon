#include "logarithmic_model.h"
#include <math.h>
#include <poincare/layout_engine.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

ExpressionLayout * LogarithmicModel::layout() {
  static ExpressionLayout * layout = LayoutEngine::createStringLayout("aln(X)+b", 8, KDText::FontSize::Small);
  return layout;
}

double LogarithmicModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a*log(x)+b;
}

double LogarithmicModel::levelSet(double * modelCoefficients, double y) const {
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
    return log(x);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: 1
    return 1;
  }
  assert(false);
  return 0.0;
}

}
