#include "exponential_model.h"
#include <math.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Regression {

Layout ExponentialModel::layout() {
  if (m_layout.isUninitialized()) {
    constexpr int size = 4;
    Layout layoutChildren[size] = {
      CodePointLayout::Builder('a', k_layoutFont),
      CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
      CodePointLayout::Builder('e', k_layoutFont),
      VerticalOffsetLayout::Builder(
          HorizontalLayout::Builder(
            CodePointLayout::Builder('b', k_layoutFont),
            CodePointLayout::Builder(UCodePointMiddleDot, k_layoutFont),
            CodePointLayout::Builder('X', k_layoutFont)
          ),
          VerticalOffsetLayoutNode::Position::Superscript
        )
    };
    m_layout = HorizontalLayout::Builder(layoutChildren, size);
  }
  return m_layout;
}

double ExponentialModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // a*e^(bx)
  return a*exp(b*x);
}

double ExponentialModel::levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (a == 0 || b == 0 || y/a <= 0) {
    return NAN;
  }
  return log(y/a)/b;
}

double ExponentialModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (derivateCoefficientIndex == 0) {
    // Derivate: exp(b*x)
    return exp(b*x);
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: a*x*exp(b*x)
    return a*x*exp(b*x);
  }
  assert(false);
  return 0.0;
}

}
