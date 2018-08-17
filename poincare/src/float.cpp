#include <poincare/float.h>
#include <poincare/allocation_failure_expression_node.h>

namespace Poincare {

template<typename T>
FloatNode<T> * FloatNode<T>::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<FloatNode<T>> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

template<typename T>
Expression FloatNode<T>::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  return Float<T>(-m_value);
}

template<typename T>
int FloatNode<T>::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == ExpressionNode::Type::Float);
  const FloatNode<T> * other = static_cast<const FloatNode<T> *>(e);
  if (value() < other->value()) {
    return -1;
  }
  if (value() > other->value()) {
    return 1;
  }
  return 0;
}

template class FloatNode<float>;
template class FloatNode<double>;

}
