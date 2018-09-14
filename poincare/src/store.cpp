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
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>


namespace Poincare {

Expression StoreNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Store(this).shallowReduce(context, angleUnit);
}

int StoreNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "\x90");
}

LayoutReference StoreNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayoutReference result = HorizontalLayoutReference();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(CharLayoutReference(Ion::Charset::Sto), result.numberOfChildren(), result.numberOfChildren(), nullptr);
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
  return context.expressionForSymbol(s.symbol()).approximateToEvaluation<T>(context, angleUnit);
}

Expression Store::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  context.setExpressionForSymbolName(value(), symbol(), context);
  Expression c1 = childAtIndex(1);
  replaceWithInPlace(c1);
  return c1.shallowReduce(context, angleUnit);
}

}
