#ifndef SIMPLIFICATION_HELPER_H
#define SIMPLIFICATION_HELPER_H

#include "expression_node.h"
#include "expression.h"

namespace Poincare {

/* Class holding helper functions (mostly defaults)
 * for expression simplication (reduce / beautify). */
class SimplificationHelper {
public:
  static void defaultDeepReduceChildren(Expression e, ExpressionNode::ReductionContext reductionContext);
  // DeepBeautify children and add parentheses if needed.
  static void deepBeautifyChildren(Expression e, ExpressionNode::ReductionContext reductionContext);
  /* Handle circuit breaker and early reduce if should be undefined
   * Returns uninitialized handle if nothing was done, the resulting expression
   * otherwise. */
  static Expression shallowReduceUndefined(Expression e);
  /* If `e` contains units, replaces with undefined to parent and returns the
   * undefined handle. Returns uninitialized handle otherwise. */
  static Expression shallowReduceBanningUnits(Expression e);
  static Expression defaultShallowReduce(Expression e);
  /* *In place* shallowReduce while keeping the units from first child.
   * The returned expression is the result with the units if units were handled.
   * Otherwise returns uninitialized handle. */
  static Expression shallowReduceKeepingUnitsFromFirstChild(Expression e, ExpressionNode::ReductionContext reductionContext);
  static Expression shallowReduceUndefinedKeepingUnitsFromFirstChild(Expression e, ExpressionNode::ReductionContext reductionContext);

  static Expression undefinedOnMatrix(Expression e, ExpressionNode::ReductionContext reductionContext);
  /* This method should be called only on expressions which have all their
   * children reduced */
  static Expression distributeReductionOverLists(Expression e, ExpressionNode::ReductionContext reductionContext);

  static bool getChildrenIfNonEmptyList(Expression e, Expression memoizedChildren[]);
};
}


#endif
