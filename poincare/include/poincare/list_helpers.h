#ifndef POINCARE_LIST_HELPERS_H
#define POINCARE_LIST_HELPERS_H

#include <poincare/list.h>

namespace Poincare {

class ListHelpers {
public:
  template <typename T> static Evaluation<T> ExtremumApproximationOfListNode(ListNode * listNode, ExpressionNode::ApproximationContext approximationContext, bool minimum);
  static Expression ExtremumOfList(List list, ExpressionNode::ReductionContext reductionContext, bool minimum);

  /* This is a Compare function (see helpers.h).
   * Context must be {ListNode *, ApproximationContext *, bool * nanIsGreatest}
   * It can also have more than 3 elements, but the first 3 must be these. */
  static bool ListEvaluationComparison(int i, int j, void * context, int numberOfElements);

private:
  static int ExtremumOfListNodeIndex(ListNode * listNode, ExpressionNode::ApproximationContext approximationContext, bool minimum);
};

}

#endif
