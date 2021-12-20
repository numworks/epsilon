#include <poincare/list_element.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

int ListElementNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, symbolChild()->name(), false, 0);
}

Layout ListElementNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  Layout nameLayout = childAtIndex(k_listChildIndex)->createLayout(floatDisplayMode, numberOfSignificantDigits);
  Layout indexLayout = childAtIndex(k_indexChildIndex)->createLayout(floatDisplayMode, numberOfSignificantDigits);
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(indexLayout, 0, true);
  result.addOrMergeChildAtIndex(nameLayout, 0, true);
  return std::move(result);
}

Expression ListElementNode::shallowReduce(ReductionContext reductionContext) {
  return ListElement(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListElementNode::templatedApproximate(ApproximationContext approximationContext) const {
  assert(false);
  return Complex<T>::Undefined();
}

Expression ListElement::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  Expression indexChild = childAtIndex(ListElementNode::k_indexChildIndex);
  Expression listChild = childAtIndex(ListElementNode::k_listChildIndex);

  if (listChild.type() != ExpressionNode::Type::List || indexChild.type() != ExpressionNode::Type::Rational) {
    return replaceWithUndefinedInPlace();
  }

  Rational rationalIndex = static_cast<Rational &>(indexChild);
  Integer integerIndex = rationalIndex.signedIntegerNumerator();
  if (!rationalIndex.isInteger() || !integerIndex.isExtractable()) {
    return replaceWithUndefinedInPlace();
  }

  int index = integerIndex.extractedInt() - 1; // List index starts at 1
  if (index < 0 || index >= listChild.numberOfChildren()) {
    return replaceWithUndefinedInPlace();
  }

  Expression element = listChild.childAtIndex(index);
  replaceWithInPlace(element);
  return element;
}

}
