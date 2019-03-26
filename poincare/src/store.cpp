#include <poincare/store.h>
#include <poincare/code_point_layout.h>
#include <poincare/context.h>
#include <poincare/complex.h>
#include <poincare/horizontal_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <ion.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}

namespace Poincare {

void StoreNode::deepReduceChildren(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  // Interrupt simplification if the expression stored contains a matrix
  if (Expression(childAtIndex(0)).recursivelyMatches([](const Expression e, Context & context, bool replaceSymbols) { return Expression::IsMatrix(e, context, replaceSymbols); }, context, true)) {
    Expression::SetInterruption(true);
  }
  return;
}

Expression StoreNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Store(this).shallowReduce(context, complexFormat, angleUnit, target);
}

int StoreNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  constexpr int stringMaxSize = CodePoint::MaxCodePointCharLength + 1;
  char string[stringMaxSize];
  SerializationHelper::CodePoint(string, stringMaxSize, UCodePointRightwardsArrow);
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, string);
}

Layout StoreNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(CodePointLayout::Builder(UCodePointRightwardsArrow), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  result.addOrMergeChildAtIndex(childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), result.numberOfChildren(), false);
  return result;
}

template<typename T>
Evaluation<T> StoreNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  /* If we are here, it means that the store node was not shallowReduced.
   * Otherwise, it would have been replaced by its symbol. We thus have to
   * setExpressionForSymbol. */
  Expression storedExpression = Store(this).storeValueForSymbol(context, complexFormat, angleUnit);
  assert(!storedExpression.isUninitialized());
  return storedExpression.node()->approximate(T(), context, complexFormat, angleUnit);
}

Expression Store::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression storedExpression = storeValueForSymbol(context, complexFormat, angleUnit);

  /* We want to replace the store with its reduced left side. If the
   * simplification of the left side failed, just replace with the left side of
   * the store without simplifying it.
   * The simplification fails for [x]->d(x) for instance, because we do not
   * have exact simplification of matrices yet. */
  bool interruptedSimplification = SimplificationHasBeenInterrupted();
  Expression reducedE = storedExpression.clone().deepReduce(context, complexFormat, angleUnit, target);
  if (!reducedE.isUninitialized() && !SimplificationHasBeenInterrupted()) {
    storedExpression = reducedE;
  }
  // Restore the previous interruption flag
  SetInterruption(interruptedSimplification);

  replaceWithInPlace(storedExpression);
  return storedExpression;
}

Expression Store::storeValueForSymbol(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Expression finalValue;
  if (symbol().type() == ExpressionNode::Type::Function) {
    // In tata + 2 ->f(tata), replace tata with xUnknown symbol
    assert(symbol().childAtIndex(0).type() == ExpressionNode::Type::Symbol);
    Expression userDefinedUnknown = symbol().childAtIndex(0);
    Symbol xUnknown = Symbol::Builder(UCodePointUnknownX);
    finalValue = childAtIndex(0).replaceSymbolWithExpression(static_cast<Symbol &>(userDefinedUnknown), xUnknown);
  } else {
    assert(symbol().type() == ExpressionNode::Type::Symbol);
    finalValue = childAtIndex(0);
  }
  assert(!finalValue.isUninitialized());
  context.setExpressionForSymbol(finalValue, symbol(), context);
  Expression storedExpression = context.expressionForSymbol(symbol(), true);

  if (storedExpression.isUninitialized()) {
    return Undefined::Builder();
  }
  if (symbol().type() == ExpressionNode::Type::Function) {
    // Replace the xUnknown symbol with the variable initially used
    assert(symbol().childAtIndex(0).type() == ExpressionNode::Type::Symbol);
    Expression userDefinedUnknown = symbol().childAtIndex(0);
    Symbol xUnknown = Symbol::Builder(UCodePointUnknownX);
    storedExpression = storedExpression.replaceSymbolWithExpression(xUnknown, static_cast<Symbol &>(userDefinedUnknown));
  }
  return storedExpression;
}

}
