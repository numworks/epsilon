#include <poincare/evaluation.h>
#include <poincare/allocation_failed_evaluation.h>
#include <poincare/expression.h>

namespace Poincare {

template<typename T>
TreeNode * EvaluationNode<T>::FailedAllocationStaticNode() {
  static AllocationFailedEvaluationNode<T> FailureNode;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&FailureNode);
  return &FailureNode;
}

template<typename T>
Expression Evaluation<T>::complexToExpression(Preferences::ComplexFormat complexFormat) const {
  return node()->complexToExpression(complexFormat);
}

}
