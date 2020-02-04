#include <poincare/store.h>
#include <poincare/complex.h>
#include <poincare/context.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

namespace Poincare {

Expression StoreNode::shallowReduce(ReductionContext reductionContext) {
  return Store(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> StoreNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  /* If we are here, it means that the store node was not shallowReduced.
   * Otherwise, it would have been replaced by its symbol. We thus have to
   * setExpressionForSymbolAbstract. */
  Expression storedExpression = Store(this).storeValueForSymbol(context, complexFormat, angleUnit);
  assert(!storedExpression.isUninitialized());
  return storedExpression.node()->approximate(T(), context, complexFormat, angleUnit);
}

Expression Store::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  // Store the expression.
  Expression storedExpression = storeValueForSymbol(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());

  if (symbol().type() == ExpressionNode::Type::Symbol) {
    /* If the symbol is not a function, we want to replace the store with its
     * reduced left side. If the simplification of the left side failed, just
     * replace with the left side of the store without simplifying it.
     * The simplification fails for [1+2]->a for instance, because we do not
     * have exact simplification of matrices yet. */
    bool interruptedSimplification = SimplificationHasBeenInterrupted();
    Expression reducedE = storedExpression.clone().deepReduce(reductionContext);
    if (!reducedE.isUninitialized() && !SimplificationHasBeenInterrupted()) {
      storedExpression = reducedE;
    }
    // Restore the previous interruption flag
    SetInterruption(interruptedSimplification);
  }

  replaceWithInPlace(storedExpression);
  return storedExpression;
}

Expression Store::storeValueForSymbol(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  assert(!value().isUninitialized());
  context->setExpressionForSymbolAbstract(value(), symbol());
  Expression storedExpression = context->expressionForSymbolAbstract(symbol(), false);

  if (storedExpression.isUninitialized()) {
    return Undefined::Builder();
  }
  return storedExpression;
}

}
