#include <poincare/uninitialized_evaluation_node.h>

namespace Poincare {

template<typename T>
UninitializedEvaluationNode<T> * UninitializedEvaluation<T>::UninitializedEvaluationStaticNode() {
  static UninitializedEvaluationNode<T> exception;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&exception);
  return &exception;
}

}
