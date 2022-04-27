#include <poincare/list_sum.h>
#include <poincare/addition.h>
#include <poincare/approximation_helper.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

const Expression::FunctionHelper ListSum::s_functionHelper;

int ListSumNode::numberOfChildren() const {
  return ListSum::s_functionHelper.numberOfChildren();
}

int ListSumNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListSum::s_functionHelper.name());
}

Layout ListSumNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListSum(this), floatDisplayMode, numberOfSignificantDigits, ListSum::s_functionHelper.name());
}

Expression ListSumNode::shallowReduce(ReductionContext reductionContext) {
  return ListSum(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListSumNode::SumOfListNode(ListNode * list, ApproximationContext approximationContext) {
  return ApproximationHelper::MapReduce<T>(
      list,
      approximationContext,
      [] (Evaluation<T> eval1, Evaluation<T> eval2, ExpressionNode::ApproximationContext approximationContext) {
      return ApproximationHelper::Reduce<T>(
          eval1,
          eval2,
          approximationContext,
          AdditionNode::computeOnComplex<T>,
          ApproximationHelper::UndefinedOnComplexAndMatrix<T>,
          ApproximationHelper::UndefinedOnMatrixAndComplex<T>,
          ApproximationHelper::UndefinedOnMatrixAndMatrix<T>
          );
      }
      );
}

template<typename T> Evaluation<T> ListSumNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }

  return SumOfListNode<T>(static_cast<ListNode *>(child), approximationContext);
}

Expression ListSum::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List) {
    return replaceWithUndefinedInPlace();
  }

  int n = child.numberOfChildren();
  Addition sum = Addition::Builder(Rational::Builder(0));
  for (int i = 0; i < n; i++) {
    sum.addChildAtIndexInPlace(child.childAtIndex(i), i, i);
  }
  replaceWithInPlace(sum);
  return sum.shallowReduce(reductionContext);
}

template Evaluation<float> ListSumNode::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListSumNode::templatedApproximate<double>(ApproximationContext approximationContext) const;

}
