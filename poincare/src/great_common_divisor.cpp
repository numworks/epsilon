#include <poincare/great_common_divisor.h>
#include <poincare/arithmetic.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

constexpr Expression::FunctionHelper GreatCommonDivisor::s_functionHelper;

Layout GreatCommonDivisorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(GreatCommonDivisor(this), floatDisplayMode, numberOfSignificantDigits, GreatCommonDivisor::s_functionHelper.name());
}

int GreatCommonDivisorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, GreatCommonDivisor::s_functionHelper.name());
}

Expression GreatCommonDivisorNode::shallowReduce(ReductionContext reductionContext) {
  return GreatCommonDivisor(this).shallowReduce(reductionContext.context());
}

Expression GreatCommonDivisorNode::shallowBeautify(ReductionContext reductionContext) {
  return GreatCommonDivisor(this).shallowBeautify(reductionContext.context());
}

template<typename T>
Evaluation<T> GreatCommonDivisorNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return Arithmetic::GCD<T>(*this, context, complexFormat, angleUnit);
}

Expression GreatCommonDivisor::shallowBeautify(Context * context) {
  /* Sort children in decreasing order:
   * gcd(1,x,x^2) --> gcd(x^2,x,1)
   * gcd(1,R(2)) --> gcd(R(2),1) */
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, false, canBeInterrupted); }, context, true, true);
  return *this;
}

Expression GreatCommonDivisor::shallowReduce(Context * context) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  assert(numberOfChildren() > 0);

  // Step 0: Merge children which are GCD
  mergeSameTypeChildrenInPlace();

  // Step 1: check that all children are compatible
  {
    Expression checkChildren = checkChildrenAreRationalIntegers(context);
    if (!checkChildren.isUninitialized()) {
      return checkChildren;
    }
  }

  // Step 2: Compute GCD
  Expression result = Arithmetic::GCD(*this);

  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> GreatCommonDivisorNode::templatedApproximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> GreatCommonDivisorNode::templatedApproximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
