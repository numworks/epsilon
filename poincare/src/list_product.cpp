#include <poincare/list_product.h>
#include <poincare/multiplication.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

int ListProductNode::numberOfChildren() const {
  return ListProduct::s_functionHelper.numberOfChildren();
}

int ListProductNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListProduct::s_functionHelper.name());
}

Layout ListProductNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListProduct(this), floatDisplayMode, numberOfSignificantDigits, ListProduct::s_functionHelper.name());
}

Expression ListProductNode::shallowReduce(ReductionContext reductionContext) {
  return ListProduct(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListProductNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }
  return static_cast<ListNode*>(child)->productOfElements<T>(approximationContext);
}

Expression ListProduct::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List) {
    return replaceWithUndefinedInPlace();
  }

  int n = child.numberOfChildren();
  Multiplication product = Multiplication::Builder(Rational::Builder(1));
  for (int i = 0; i < n; i++) {
    product.addChildAtIndexInPlace(child.childAtIndex(i), i, i);
  }
  replaceWithInPlace(product);
  return product.shallowReduce(reductionContext);
}

template Evaluation<float> ListProductNode::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListProductNode::templatedApproximate<double>(ApproximationContext approximationContext) const;

}
