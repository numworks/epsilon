#include <poincare/list_helpers.h>
#include <poincare/helpers.h>
#include <poincare/expression_node.h>
#include <poincare/undefined.h>

namespace Poincare {

template <typename T> Evaluation<T> ListHelpers::ExtremumApproximationOfListNode(ListNode * listNode, ExpressionNode::ApproximationContext approximationContext, bool minimum) {
    if (listNode->numberOfChildren() == 0) {
      return Complex<T>::Undefined();
    }
    return listNode->childAtIndex(ExtremumOfListNodeIndex(listNode, approximationContext, minimum))->approximate(static_cast<T>(0), approximationContext);
}

Expression ListHelpers::ExtremumOfList(List list, ExpressionNode::ReductionContext reductionContext, bool minimum) {
  if (list.numberOfChildren() == 0) {
    return Undefined::Builder();
  }
  ExpressionNode::ApproximationContext approximationContext(reductionContext, true);
  return list.childAtIndex(ExtremumOfListNodeIndex(list.node(), approximationContext, minimum));
}

bool ListHelpers::ListEvaluationComparison(int i, int j, void * context, int numberOfElements) {
  void ** c = reinterpret_cast<void **>(context);
  ListNode * list = reinterpret_cast<ListNode *>(c[0]);
  ExpressionNode::ApproximationContext * approximationContext = reinterpret_cast<ExpressionNode::ApproximationContext *>(c[1]);
  bool * nanIsGreatest = reinterpret_cast<bool *>(c[2]);
  float xI = list->childAtIndex(i)->approximate(static_cast<float>(0), *approximationContext).toScalar();
  float xJ =  list->childAtIndex(j)->approximate(static_cast<float>(0), *approximationContext).toScalar();
  return Poincare::Helpers::FloatComparison(xI, xJ, *nanIsGreatest);
}

int ListHelpers::ExtremumOfListNodeIndex(ListNode * listNode, ExpressionNode::ApproximationContext approximationContext, bool minimum) {
  void * comparisonContext[] = {listNode, &approximationContext, &minimum};
  return Poincare::Helpers::ExtremumIndex(
      ListEvaluationComparison,
      comparisonContext,
      listNode->numberOfChildren(),
      minimum);
}

template Evaluation<float>  ListHelpers::ExtremumApproximationOfListNode<float>(ListNode * list, ExpressionNode::ApproximationContext approximationContext, bool minimum);
template Evaluation<double>  ListHelpers::ExtremumApproximationOfListNode<double>(ListNode * list, ExpressionNode::ApproximationContext approximationContext, bool minimum);


}
