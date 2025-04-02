#include <assert.h>
#include <ion/circuit_breaker.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/expression.h>
#include <poincare/old/complex.h>
#include <poincare/old/context.h>
#include <poincare/old/store.h>
#include <poincare/old/symbol.h>
#include <poincare/old/undefined.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/memory/tree.h>
#include <stdlib.h>

#include <cmath>

namespace Poincare {

OExpression StoreNode::shallowReduce(const ReductionContext& reductionContext) {
  return OStore(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> StoreNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  /* We return a dummy value if the store is interrupted. Since the app waits
   * for a Store node to do the actual store, there is nothing better to do. */
  return Complex<T>::Undefined();
}

void OStore::deepReduceChildren(const ReductionContext& reductionContext) {
  // Only the value of a symbol should have no free variables
  if (symbol().otype() == ExpressionNode::Type::Symbol) {
    childAtIndex(0).deepReduce(reductionContext);
  }
}

OExpression OStore::shallowReduce(ReductionContext reductionContext) {
  /* Stores are kept by the reduction and the app will do the effective store if
   * deemed necessary. Side-effects of the storage modification will therefore
   * happen outside of the checkpoint. */
  return *this;
}

}  // namespace Poincare
