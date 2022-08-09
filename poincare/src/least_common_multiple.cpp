#include <poincare/least_common_multiple.h>
#include <poincare/arithmetic.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

constexpr Expression::FunctionHelper LeastCommonMultiple::s_functionHelper;

Layout LeastCommonMultipleNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(LeastCommonMultiple(this), floatDisplayMode, numberOfSignificantDigits, LeastCommonMultiple::s_functionHelper.aliasesList().mainAlias(), context);
}

int LeastCommonMultipleNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, LeastCommonMultiple::s_functionHelper.aliasesList().mainAlias());
}

Expression LeastCommonMultipleNode::shallowReduce(const ReductionContext& reductionContext) {
  return LeastCommonMultiple(this).shallowReduce(reductionContext);
}

Expression LeastCommonMultipleNode::shallowBeautify(const ReductionContext& reductionContext) {
  return LeastCommonMultiple(this).shallowBeautify(reductionContext.context());
}

template<typename T>
Evaluation<T> LeastCommonMultipleNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  /* TODO : distribute approx over list with Map */
  return Arithmetic::LCM<T>(*this, approximationContext);
}

Expression LeastCommonMultiple::shallowBeautify(Context * context) {
  /* Sort children in decreasing order:
   * lcm(1,x,x^2) --> lcm(x^2,x,1)
   * lcm(1,R(2)) --> lcm(R(2),1) */
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2) { return ExpressionNode::SimplificationOrder(e1, e2, false); }, context, true, false);
  return *this;
}

Expression LeastCommonMultiple::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  assert(numberOfChildren() > 0);

  // Step 0: Merge children which are LCM
  mergeSameTypeChildrenInPlace();

  // Step 1: check that all children are compatible
  {
    Expression checkChildren = checkChildrenAreRationalIntegersAndUpdate(reductionContext);
    if (!checkChildren.isUninitialized()) {
      return checkChildren;
    }
  }

  // Step 2: Compute LCM
  Expression result = Arithmetic::LCM(*this);

  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> LeastCommonMultipleNode::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> LeastCommonMultipleNode::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

}
