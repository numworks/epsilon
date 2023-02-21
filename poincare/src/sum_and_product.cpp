#include <poincare/decimal.h>
#include <poincare/simplification_helper.h>
#include <poincare/sum_and_product.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

Layout SumAndProductNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  return createSumAndProductLayout(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits,
                                    context),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits,
                                    context),
      childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits,
                                    context),
      childAtIndex(3)->createLayout(floatDisplayMode, numberOfSignificantDigits,
                                    context));
}

Expression SumAndProductNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return SumAndProduct(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> SumAndProductNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  // TODO : Reduction distributes on list but not approx
  if (type() == ExpressionNode::Type::Sum &&
      Poincare::Preferences::sharedPreferences->sumIsForbidden()) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> aInput =
      childAtIndex(2)->approximate(T(), approximationContext);
  Evaluation<T> bInput =
      childAtIndex(3)->approximate(T(), approximationContext);
  T start = aInput.toScalar();
  T end = bInput.toScalar();
  if (std::isnan(start) || std::isnan(end) || start != (int)start ||
      end != (int)end || end - start > k_maxNumberOfSteps) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> result =
      Complex<T>::Builder(static_cast<T>(emptySumAndProductValue()));
  for (int i = (int)start; i <= (int)end; i++) {
    result = evaluateWithNextTerm(T(), result,
                                  approximateFirstChildWithArgument(
                                      static_cast<T>(i), approximationContext),
                                  approximationContext.complexFormat());
    if (result.isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return result;
}

Expression SumAndProduct::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  if (type() == ExpressionNode::Type::Sum &&
      Poincare::Preferences::sharedPreferences->sumIsForbidden()) {
    return replaceWithUndefinedInPlace();
  }
  return *this;
}

template Evaluation<float> SumAndProductNode::templatedApproximate(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> SumAndProductNode::templatedApproximate(
    const ApproximationContext& approximationContext) const;

}  // namespace Poincare
