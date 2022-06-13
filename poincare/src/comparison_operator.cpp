#include <poincare/comparison_operator.h>
#include <poincare/subtraction.h>
#include <poincare/code_point_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/horizontal_layout.h>

namespace Poincare {

Layout ComparisonOperatorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  Layout operatorLayout = CodePointLayout::Builder(comparisonCodePoint());
  operatorLayout.setMargin(true);
  result.addChildAtIndex(operatorLayout, result.numberOfChildren(), result.numberOfChildren(), nullptr);
  Layout secondChildLayout = childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits);
  secondChildLayout.setMargin(true);
  result.addOrMergeChildAtIndex(secondChildLayout, result.numberOfChildren(), false);
  return std::move(result);
}

int ComparisonOperatorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, comparisonString());
}

Evaluation<float> ComparisonOperatorNode::approximate(SinglePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<float>(approximationContext);
}

Evaluation<double> ComparisonOperatorNode::approximate(DoublePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<double>(approximationContext);
}

template<typename T>
Evaluation<T> ComparisonOperatorNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return Complex<T>::Undefined();
}

bool ComparisonOperator::IsComparisonOperatorType(ExpressionNode::Type type) {
  switch (type) {
  case ExpressionNode::Type::Equal:
  case ExpressionNode::Type::Superior:
  case ExpressionNode::Type::SuperiorEqual:
  case ExpressionNode::Type::Inferior:
  case ExpressionNode::Type::InferiorEqual:
    return true;
  default:
    return false;
  }
}

ExpressionNode::Type ComparisonOperator::Opposite(ExpressionNode::Type type) {
  switch (type) {
  case ExpressionNode::Type::Superior:
    return ExpressionNode::Type::Inferior;
  case ExpressionNode::Type::SuperiorEqual:
    return ExpressionNode::Type::InferiorEqual;
  case ExpressionNode::Type::Inferior:
    return ExpressionNode::Type::Superior;
  default:
    assert(type == ExpressionNode::Type::InferiorEqual);
    return ExpressionNode::Type::SuperiorEqual;
  }
}

Expression ComparisonOperator::standardEquation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::ReductionTarget reductionTarget) const {
  Expression sub = Subtraction::Builder(childAtIndex(0).clone(), childAtIndex(1).clone());
  return sub.cloneAndReduce(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, reductionTarget));
}

}
