#include <poincare/sum_and_product.h>
#include <poincare/decimal.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>
#include <poincare/simplification_helper.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

Layout SumAndProductNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return createSumAndProductLayout(
    childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
    childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits),
    childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits),
    childAtIndex(3)->createLayout(floatDisplayMode, numberOfSignificantDigits)
  );
}

Expression SumAndProductNode::shallowReduce(ReductionContext reductionContext) {
  return SumAndProduct(this).shallowReduce(reductionContext.context());
}

template<typename T>
Evaluation<T> SumAndProductNode::templatedApproximate(ApproximationContext approximationContext) const {
  if (type() == ExpressionNode::Type::Sum && Poincare::Preferences::sharedPreferences()->sumIsForbidden()) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> aInput = childAtIndex(2)->approximate(T(), approximationContext);
  Evaluation<T> bInput = childAtIndex(3)->approximate(T(), approximationContext);
  T start = aInput.toScalar();
  T end = bInput.toScalar();
  if (std::isnan(start) || std::isnan(end) || start != (int)start || end != (int)end || end - start > k_maxNumberOfSteps) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> result = Complex<T>::Builder(static_cast<T>(emptySumAndProductValue()));
  for (int i = (int)start; i <= (int)end; i++) {
    result = evaluateWithNextTerm(T(), result, approximateFirstChildWithArgument(static_cast<T>(i), approximationContext), approximationContext.complexFormat());
    if (result.isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return result;
}

Expression SumAndProduct::shallowReduce(Context * context) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  if (type() == ExpressionNode::Type::Sum && Poincare::Preferences::sharedPreferences()->sumIsForbidden()) {
    return replaceWithUndefinedInPlace();
  }
  assert(!childAtIndex(1).deepIsMatrix(context));
  if (childAtIndex(2).deepIsMatrix(context) || childAtIndex(3).deepIsMatrix(context)) {
    return replaceWithUndefinedInPlace();
  }
  return *this;
}

template Evaluation<float> SumAndProductNode::templatedApproximate(ApproximationContext approximationContext) const;
template Evaluation<double> SumAndProductNode::templatedApproximate(ApproximationContext approximationContext) const;

}
