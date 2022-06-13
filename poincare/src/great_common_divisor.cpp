#include <poincare/great_common_divisor.h>
#include <poincare/arithmetic.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

constexpr Expression::FunctionHelper GreatCommonDivisor::s_functionHelper;

Layout GreatCommonDivisorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(GreatCommonDivisor(this), floatDisplayMode, numberOfSignificantDigits, GreatCommonDivisor::s_functionHelper.name());
}

int GreatCommonDivisorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, GreatCommonDivisor::s_functionHelper.name());
}

Expression GreatCommonDivisorNode::shallowReduce(const ReductionContext& reductionContext) {
  return GreatCommonDivisor(this).shallowReduce(reductionContext);
}

Expression GreatCommonDivisorNode::shallowBeautify(const ReductionContext& reductionContext) {
  return GreatCommonDivisor(this).shallowBeautify(reductionContext.context());
}

template<typename T>
Evaluation<T> GreatCommonDivisorNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  /* TODO : distribute approx over list with Map */
  return Arithmetic::GCD<T>(*this, approximationContext);
}

Expression GreatCommonDivisor::shallowBeautify(Context * context) {
  /* Sort children in decreasing order:
   * gcd(1,x,x^2) --> gcd(x^2,x,1)
   * gcd(1,R(2)) --> gcd(R(2),1) */
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2) { return ExpressionNode::SimplificationOrder(e1, e2, false); }, context, true);
  return *this;
}

Expression GreatCommonDivisor::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  assert(numberOfChildren() > 0);

  // Step 0: Merge children which are GCD
  mergeSameTypeChildrenInPlace();

  // Step 1: check that all children are compatible
  {
    Expression checkChildren = checkChildrenAreRationalIntegersAndUpdate(reductionContext);
    if (!checkChildren.isUninitialized()) {
      return checkChildren;
    }
  }

  // Step 2: Compute GCD
  Expression result = Arithmetic::GCD(*this);

  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> GreatCommonDivisorNode::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> GreatCommonDivisorNode::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

}
