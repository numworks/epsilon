#include "quadratic_model.h"
#include "../../shared/poincare_helpers.h"
#include <math.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/decimal.h>
#include <poincare/number.h>
#include <poincare/symbol.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout QuadraticModel::layout() {
  if (m_layout.isUninitialized()) {
    constexpr int size = 10;
    Layout layoutChildren[size] = {
      CodePointLayout::Builder('a', KDFont::SmallFont),
      CodePointLayout::Builder(UCodePointMiddleDot, KDFont::SmallFont),
      CodePointLayout::Builder('X', KDFont::SmallFont),
      VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('2', KDFont::SmallFont),
          VerticalOffsetLayoutNode::Type::Superscript
        ),
      CodePointLayout::Builder('+', KDFont::SmallFont),
      CodePointLayout::Builder('b', KDFont::SmallFont),
      CodePointLayout::Builder(UCodePointMiddleDot, KDFont::SmallFont),
      CodePointLayout::Builder('X', KDFont::SmallFont),
      CodePointLayout::Builder('+', KDFont::SmallFont),
      CodePointLayout::Builder('c', KDFont::SmallFont),
    };
    m_layout = HorizontalLayout::Builder(layoutChildren, size);
  }
  return m_layout;
}

Expression QuadraticModel::simplifiedExpression(double * modelCoefficients, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  // a*x^2+b*x+c
  Expression addChildren[] = {
    Multiplication::Builder(
      Number::DecimalNumber(a),
      Power::Builder(
        Symbol::Builder('x'),
        Decimal::Builder(2.0))),
    Multiplication::Builder(
      Number::DecimalNumber(b),
      Symbol::Builder('x')),
    Number::DecimalNumber(c)
  };
  Expression result = Addition::Builder(addChildren, 3);
  PoincareHelpers::Simplify(&result, *context);
  return result;
}

double QuadraticModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  return a*x*x+b*x+c;
}

double QuadraticModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 0) {
    // Derivate: x^2
    return x*x;
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate: x
    return x;
  }
  if (derivateCoefficientIndex == 2) {
    // Derivate: 1
    return 1;
  }
  assert(false);
  return 0.0;
}

}
