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
#include <poincare/code_point_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/horizontal_layout.h>
#include <ion.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
}
#include <utility>

namespace Poincare {

Expression EqualNode::shallowReduce(ReductionContext reductionContext) {
  return Equal(this).shallowReduce();
}

Layout EqualNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(CodePointLayout::Builder('='), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  result.addOrMergeChildAtIndex(childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), result.numberOfChildren(), false);
  return std::move(result);
}

int EqualNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "=");
}

template<typename T>
Evaluation<T> EqualNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return Complex<T>::Undefined();
}


Expression Equal::standardEquation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Expression sub = Subtraction::Builder(childAtIndex(0).clone(), childAtIndex(1).clone());
  /* When reducing the equation, we specify the reduction target to be
   * SystemForAnalysis. This enables to expand Newton multinom to be able to
   * detect polynom correctly ("(x+2)^2" in this form won't be detected
   * unless expanded). */
  return sub.reduce(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::SystemForAnalysis));
}

Expression Equal::shallowReduce() {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  if (childAtIndex(0).isIdenticalTo(childAtIndex(1))) {
    Expression result = Rational::Builder(1);
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

}
