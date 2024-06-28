#include <poincare/layout.h>
#include <poincare/old/list_product.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

int ListProductNode::numberOfChildren() const {
  return ListProduct::s_functionHelper.numberOfChildren();
}

size_t ListProductNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ListProduct::s_functionHelper.aliasesList().mainAlias());
}

OExpression ListProductNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListProduct(this).shallowReduce(reductionContext);
}

OExpression ListProduct::shallowReduce(ReductionContext reductionContext) {
  OExpression child = childAtIndex(0);
  if (child.otype() != ExpressionNode::Type::OList) {
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

}  // namespace Poincare
