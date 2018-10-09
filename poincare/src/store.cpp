#include <poincare/store.h>
#include <poincare/char_layout.h>
#include <poincare/context.h>
#include <poincare/complex.h>
#include <poincare/horizontal_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol.h>
#include <ion.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}

namespace Poincare {

Expression StoreNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Store(this).shallowReduce(context, angleUnit, replaceSymbols);
}

int StoreNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "\x90");
}

Layout StoreNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result = HorizontalLayout();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(CharLayout(Ion::Charset::Sto), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  result.addOrMergeChildAtIndex(childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), result.numberOfChildren(), false);
  return result;
}

template<typename T>
Evaluation<T> StoreNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  /* If we are here, it means that the store node was not shallowReduced.
   * Otherwise, it would have been replaced by its symbol. We thus have to
   * setExpressionForSymbol. */
  Store s(this);
  context.setExpressionForSymbol(s.value(), s.symbol(), context);
  Expression e = context.expressionForSymbol(s.symbol());
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.approximateToEvaluation<T>(context, angleUnit);
}

Expression Store::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  Expression finalValue;
  if (symbol().type() == ExpressionNode::Type::Function) {
    // In tata + 2 ->f(tata), replace tata with xUnknown symbol
    assert(symbol().childAtIndex(0).type() == ExpressionNode::Type::Symbol);
    Expression userDefinedUnknown = symbol().childAtIndex(0);
    const char x[2] = {Symbol::SpecialSymbols::UnknownX, 0};
    Symbol xUnknown = Symbol(x, 1);
    finalValue = childAtIndex(0).replaceSymbolWithExpression(static_cast<Symbol &>(userDefinedUnknown), xUnknown);
  } else {
    assert(symbol().type() == ExpressionNode::Type::Symbol);
    finalValue = childAtIndex(0);
  }
  finalValue = finalValue.deepReduce(context, angleUnit, false);
  context.setExpressionForSymbol(finalValue, symbol(), context);
  Expression c1 = childAtIndex(1);
  replaceWithInPlace(c1);
  return c1.shallowReduce(context, angleUnit);
}

}
