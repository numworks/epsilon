extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
}
#include <poincare/equal.h>
#include <ion.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>

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

Expression * Equal::standardEquation(Context & context, Preferences::AngleUnit angleUnit) const {
  Expression * sub = new Subtraction(operand(0), operand(1), true);
  Reduce(&sub, context, angleUnit);
  return sub;
}

Expression Equal::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression * e = Expression::defaultShallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  if (operand(0)->isIdenticalTo(operand(1))) {
    return replaceWith(RationalReference(1), true);
  }
  return this;
}

LayoutRef Equal::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayoutRef result;
  result.addOrMergeChildAtIndex(operand(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(CharLayoutRef('='), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  result.addOrMergeChildAtIndex(operand(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), result.numberOfChildren(), false);
  return result;
}

template<typename T>
Evaluation<T> Equal::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return new Complex<T>(Complex<T>::Undefined());
}

}
