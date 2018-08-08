#include <poincare/float.h>
#include <poincare/allocation_failure_expression_node.h>

namespace Poincare {

template<typename T> FloatNode<T> * FloatNode<T>::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<FloatNode<T>> failure;
  return &failure;
}

template class FloatNode<float>;
template class FloatNode<double>;

}
