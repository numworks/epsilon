#include <assert.h>
#include <ion/circuit_breaker.h>
#include <math.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/complex.h>
#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/store.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <stdlib.h>

namespace Poincare {

bool Store::storeValueForSymbol(Context* context) const {
  assert(!value().isUninitialized());
  return context->setExpressionForSymbolAbstract(value(), symbol());
}

#define Store OStore

OExpression StoreNode::shallowReduce(const ReductionContext& reductionContext) {
  return Store(this).shallowReduce(reductionContext);
}

OExpression StoreNode::deepReplaceReplaceableSymbols(
    Context* context, TrinaryBoolean* isCircular, int parameteredAncestorsCount,
    SymbolicComputation symbolicComputation) {
  return Store(this).deepReplaceReplaceableSymbols(
      context, isCircular, parameteredAncestorsCount, symbolicComputation);
}

template <typename T>
Evaluation<T> StoreNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  /* We return a dummy value if the store is interrupted. Since the app waits
   * for a Store node to do the actual store, there is nothing better to do. */
  return Complex<T>::Undefined();
}

void Store::deepReduceChildren(const ReductionContext& reductionContext) {
  // Only the value of a symbol should have no free variables
  if (symbol().otype() == ExpressionNode::Type::Symbol) {
    childAtIndex(0).deepReduce(reductionContext);
  }
}

OExpression Store::shallowReduce(ReductionContext reductionContext) {
  /* Stores are kept by the reduction and the app will do the effective store if
   * deemed necessary. Side-effects of the storage modification will therefore
   * happen outside of the checkpoint. */
  return *this;
}

bool Store::storeValueForSymbol(Context* context) const {
  assert(!value().isUninitialized());
  return context->setExpressionForSymbolAbstract(value(), symbol());
}

OExpression Store::deepReplaceReplaceableSymbols(
    Context* context, TrinaryBoolean* isCircular, int parameteredAncestorsCount,
    SymbolicComputation symbolicComputation) {
  // Only the value of a symbol should have no free variables
  if (symbol().otype() == ExpressionNode::Type::Symbol) {
    OExpression value = childAtIndex(0).deepReplaceReplaceableSymbols(
        context, isCircular, parameteredAncestorsCount, symbolicComputation);
    replaceChildAtIndexInPlace(0, value);
  }
  return *this;
}

}  // namespace Poincare
