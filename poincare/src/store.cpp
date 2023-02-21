#include <assert.h>
#include <ion/circuit_breaker.h>
#include <math.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/complex.h>
#include <poincare/context.h>
#include <poincare/store.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <stdlib.h>

namespace Poincare {

Expression StoreNode::shallowReduce(const ReductionContext& reductionContext) {
  return Store(this).shallowReduce(reductionContext);
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
  if (symbol().type() == ExpressionNode::Type::Symbol) {
    childAtIndex(0).deepReduce(reductionContext);
  }
}

Expression Store::shallowReduce(ReductionContext reductionContext) {
  /* Stores are kept by the reduction and the app will do the effective store if
   * deemed necessary. Side-effects of the storage modification will therefore
   * happen outside of the checkpoint. */
  return *this;
}

bool Store::storeValueForSymbol(Context* context) const {
  assert(!value().isUninitialized());
  return context->setExpressionForSymbolAbstract(value(), symbol());
}

}  // namespace Poincare
