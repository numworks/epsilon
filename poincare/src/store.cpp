extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}
#include <poincare/store.h>
#include <ion.h>
#include <poincare/context.h>
#include <poincare/serialization_helper.h>
#include <poincare/complex.h>
#include <poincare/allocation_failure_expression_node.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>


namespace Poincare {

StoreNode * StoreNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<StoreNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

Expression StoreNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  return Store(this).shallowReduce(context, angleUnit, futureParent);
}

int StoreNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "\x90");
}

LayoutRef StoreNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayoutRef result = HorizontalLayoutRef();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(CharLayoutRef(Ion::Charset::Sto), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  result.addOrMergeChildAtIndex(childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), result.numberOfChildren(), false);
  return result;
}

template<typename T>
Evaluation<T> StoreNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Store s(this);
  context.setExpressionForSymbolName(s.value(), s.symbol(), context);
  if (context.expressionForSymbol(s.symbol()).isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return context.expressionForSymbol(s.symbol()).node()->approximate(T(), context, angleUnit);
}

Expression Store::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  context.setExpressionForSymbolName(value(), symbol(), context);
  return symbol().shallowReduce(context, angleUnit);
}

}
