extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
}
#include <poincare/equal.h>
#include <ion.h>
#include <poincare/complex.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include "layout/char_layout.h"
#include "layout/horizontal_layout.h"

namespace Poincare {

Expression::Type Equal::type() const {
  return Type::Equal;
}

Expression * Equal::clone() const {
  return new Equal(operands(), true);
}

int Equal::polynomialDegree(char symbolName) const {
  return -1;
}

Expression * Equal::standardEquation(Context & context, AngleUnit angleUnit) const {
  Expression * sub = new Subtraction(operand(0), operand(1), true);
  Reduce(&sub, context, angleUnit);
  return sub;
}

Expression * Equal::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  if (operand(0)->isIdenticalTo(operand(1))) {
    return replaceWith(new Rational(1), true);
  }
  return this;
}

ExpressionLayout * Equal::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  HorizontalLayout * result = new HorizontalLayout();
  result->addOrMergeChildAtIndex(operand(0)->createLayout(floatDisplayMode, complexFormat), 0, false);
  result->addChildAtIndex(new CharLayout('='), result->numberOfChildren());
  result->addOrMergeChildAtIndex(operand(1)->createLayout(floatDisplayMode, complexFormat), result->numberOfChildren(), false);
  return result;
}

template<typename T>
Expression * Equal::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  return new Complex<T>(Complex<T>::Float(NAN));
}

}
