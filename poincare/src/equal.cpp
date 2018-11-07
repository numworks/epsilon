#include <poincare/equal.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/char_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/horizontal_layout.h>
#include <ion.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
}
namespace Poincare {

Expression EqualNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Equal(this).shallowReduce(context, angleUnit, replaceSymbols);
}

Layout EqualNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result;
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(CharLayout('='), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  result.addOrMergeChildAtIndex(childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), result.numberOfChildren(), false);
  return result;
}

int EqualNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "=");
}

template<typename T>
Evaluation<T> EqualNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return Complex<T>::Undefined();
}

Expression Equal::standardEquation(Context & context, Preferences::AngleUnit angleUnit) const {
  Expression sub = Subtraction(childAtIndex(0).clone(), childAtIndex(1).clone());
  return sub.reduce(context, angleUnit);
}

Expression Equal::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
  if (childAtIndex(0).isIdenticalTo(childAtIndex(1))) {
    Expression result = Rational(1);
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

}
