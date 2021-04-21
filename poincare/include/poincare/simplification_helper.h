#ifndef SIMPLIFICATOIN_HELPER_H
#define SIMPLIFICATOIN_HELPER_H

#include "expression_node.h"
#include "expression.h"

namespace Poincare {

/* Namespace holding helper functions (mostly defaults)
* for expression simplication (reduce / beautify). */
class SimplificationHelper {
public:
  static void defaultDeepReduceChildren(Expression e, ExpressionNode::ReductionContext reductionContext);
  static void defaultDeepBeautifyChildren(Expression e, ExpressionNode::ReductionContext reductionContext);
  static Expression defaultShallowReduce(Expression e);
  static Expression defaultHandleUnitsInChildren(Expression e); // Children must be reduced
  /* *In place* shallowReduce while keeping the units.
  * `handledUnits` is set to true if units were handled, in which case
  * the returned expression is the result with the units.
  * Otherwise simply returns *this.
  * Warning: this function will handle units only for the first child.*/
  static Expression shallowReducePotentialUnit(Expression e, ExpressionNode::ReductionContext reductionContext, bool * handledUnits);

};
}


#endif /* SIMPLIFICATOIN_HELPER_H */
