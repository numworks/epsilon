#include "quartic_model.h"

#include <assert.h>
#include <poincare/based_integer.h>
#include <poincare/code_point_layout.h>
#include <poincare/decimal.h>
#include <poincare/horizontal_layout.h>
#include <poincare/multiplication.h>
#include <poincare/number.h>
#include <poincare/power.h>
#include <poincare/print.h>
#include <poincare/symbol.h>
#include <poincare/vertical_offset_layout.h>

#include "../../shared/poincare_helpers.h"

using namespace Poincare;
using namespace Shared;

namespace Regression {

Layout QuarticModel::templateLayout() const {
  return HorizontalLayout::Builder(
      {CodePointLayout::Builder('a'),
       CodePointLayout::Builder(UCodePointMiddleDot),
       CodePointLayout::Builder('x'),
       VerticalOffsetLayout::Builder(
           CodePointLayout::Builder('4'),
           VerticalOffsetLayoutNode::VerticalPosition::Superscript),
       CodePointLayout::Builder('+'),
       CodePointLayout::Builder('b'),
       CodePointLayout::Builder(UCodePointMiddleDot),
       CodePointLayout::Builder('x'),
       VerticalOffsetLayout::Builder(
           CodePointLayout::Builder('3'),
           VerticalOffsetLayoutNode::VerticalPosition::Superscript),
       CodePointLayout::Builder('+'),
       CodePointLayout::Builder('c'),
       CodePointLayout::Builder(UCodePointMiddleDot),
       CodePointLayout::Builder('x'),
       VerticalOffsetLayout::Builder(
           CodePointLayout::Builder('2'),
           VerticalOffsetLayoutNode::VerticalPosition::Superscript),
       CodePointLayout::Builder('+'),
       CodePointLayout::Builder('d'),
       CodePointLayout::Builder(UCodePointMiddleDot),
       CodePointLayout::Builder('x'),
       CodePointLayout::Builder('+'),
       CodePointLayout::Builder('e')});
}

Expression QuarticModel::privateExpression(double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double e = modelCoefficients[4];
  // a*x^4+b*x^3+c*x^2+d*x+e
  return AdditionOrSubtractionBuilder(
      AdditionOrSubtractionBuilder(
          AdditionOrSubtractionBuilder(
              AdditionOrSubtractionBuilder(
                  Multiplication::Builder(
                      Number::DecimalNumber(a),
                      Power::Builder(Symbol::Builder(k_xSymbol),
                                     BasedInteger::Builder(4))),
                  Multiplication::Builder(
                      Number::DecimalNumber(std::fabs(b)),
                      Power::Builder(Symbol::Builder(k_xSymbol),
                                     BasedInteger::Builder(3))),
                  b >= 0.0),
              Multiplication::Builder(Number::DecimalNumber(std::fabs(c)),
                                      Power::Builder(Symbol::Builder(k_xSymbol),
                                                     BasedInteger::Builder(2))),
              c >= 0.0),
          Multiplication::Builder(Number::DecimalNumber(std::fabs(d)),
                                  Symbol::Builder(k_xSymbol)),
          d >= 0.0),
      Number::DecimalNumber(std::fabs(e)), e >= 0.0);
}

double QuarticModel::evaluate(double* modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double e = modelCoefficients[4];
  return a * x * x * x * x + b * x * x * x + c * x * x + d * x + e;
}

double QuarticModel::partialDerivate(double* modelCoefficients,
                                     int derivateCoefficientIndex,
                                     double x) const {
  switch (derivateCoefficientIndex) {
    case 0:
      // Derivate with respect to a: x^4
      return x * x * x * x;
    case 1:
      // Derivate with respect to b: x^3
      return x * x * x;
    case 2:
      // Derivate with respect to c: x^2
      return x * x;
    case 3:
      // Derivate with respect to d: x
      return x;
    default:
      assert(derivateCoefficientIndex == 4);
      // Derivate with respect to e: 1
      return 1.0;
  };
}

}  // namespace Regression
