#include <poincare/nth_root.h>
#include <poincare/addition.h>
#include <poincare/constant.h>
#include <poincare/division.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/power.h>
#include <poincare/undefined.h>
#include <poincare/nth_root_layout.h>
#include <poincare/subtraction.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper NthRoot::s_functionHelper;

int NthRootNode::numberOfChildren() const { return NthRoot::s_functionHelper.numberOfChildren(); }

Layout NthRootNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return NthRootLayout(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int NthRootNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NthRoot::s_functionHelper.name());
}

Expression NthRootNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return NthRoot(this).shallowReduce(context, complexFormat, angleUnit, target);
}

template<typename T>
Evaluation<T> NthRootNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  NthRoot e(this);
  /* Turn the root(x,n) into x^(1/n) to use the approximation on power (which
   * takes into account the complex format to force to return the real
   * approximation when it exists instead of returning the principale value). */
  Expression equivalence = Power(e.childAtIndex(0).clone(), Division(Rational(1), e.childAtIndex(1).clone()));
  return equivalence.node()->approximate(T(), context, complexFormat, angleUnit);
}

Expression NthRoot::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix || childAtIndex(1).type() == ExpressionNode:Type::Matrix) {
    return Undefined();
  }
#endif
  Expression invIndex = Power(childAtIndex(1), Rational(-1));
  Power p = Power(childAtIndex(0), invIndex);
  invIndex.shallowReduce(context, complexFormat, angleUnit, target);
  replaceWithInPlace(p);
  return p.shallowReduce(context, complexFormat, angleUnit, target);
}

}
