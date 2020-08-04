#include <poincare/least_common_multiple.h>
#include <poincare/arithmetic.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

constexpr Expression::FunctionHelper LeastCommonMultiple::s_functionHelper;

Layout LeastCommonMultipleNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(LeastCommonMultiple(this), floatDisplayMode, numberOfSignificantDigits, LeastCommonMultiple::s_functionHelper.name());
}

int LeastCommonMultipleNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, LeastCommonMultiple::s_functionHelper.name());
}

Expression LeastCommonMultipleNode::shallowReduce(ReductionContext reductionContext) {
  return LeastCommonMultiple(this).shallowReduce(reductionContext.context());
}

Expression LeastCommonMultipleNode::shallowBeautify(ReductionContext reductionContext) {
  return LeastCommonMultiple(this).shallowBeautify(reductionContext.context());
}

template<typename T>
Evaluation<T> LeastCommonMultipleNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return Arithmetic::LCM<T>(*this, context, complexFormat, angleUnit);
}

Expression LeastCommonMultiple::shallowBeautify(Context * context) {
  /* Sort children in decreasing order:
   * lcm(1,x,x^2) --> lcm(x^2,x,1)
   * lcm(1,R(2)) --> lcm(R(2),1) */
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, false, canBeInterrupted); }, context, true, true);
  return *this;
}

Expression LeastCommonMultiple::shallowReduce(Context * context) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  assert(numberOfChildren() > 0);

  // Step 0: Merge children which are LCM
  mergeSameTypeChildrenInPlace();

  // Step 1: check that all children are compatible
  {
    Expression checkChildren = checkChildrenAreRationalIntegers(context);
    if (!checkChildren.isUninitialized()) {
      return checkChildren;
    }
  }

  // Step 2: Compute LCM
  Expression result = Arithmetic::LCM(*this);

  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> LeastCommonMultipleNode::templatedApproximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> LeastCommonMultipleNode::templatedApproximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
