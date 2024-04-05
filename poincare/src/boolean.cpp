#include <poincare/boolean.h>
#include <poincare/layout.h>

namespace Poincare {

template <typename T>
OExpression BooleanEvaluationNode<T>::complexToExpression(
    Preferences::Preferences::ComplexFormat complexFormat) const {
  return OBoolean::Builder(value());
}

template <typename T>
BooleanEvaluation<T> BooleanEvaluation<T>::Builder(bool value) {
  void *bufferNode = Pool::sharedPool->alloc(sizeof(BooleanEvaluationNode<T>));
  BooleanEvaluationNode<T> *node =
      new (bufferNode) BooleanEvaluationNode<T>(value);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<BooleanEvaluation<T> &>(h);
}

template class BooleanEvaluationNode<float>;
template class BooleanEvaluationNode<double>;
template BooleanEvaluation<float> BooleanEvaluation<float>::Builder(bool value);
template BooleanEvaluation<double> BooleanEvaluation<double>::Builder(
    bool value);

size_t BooleanNode::serialize(char *buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return strlcpy(buffer, aliasesList().mainAlias(), bufferSize);
}

OBoolean OBoolean::Builder(bool value) {
  void *bufferNode = Pool::sharedPool->alloc(sizeof(BooleanNode));
  BooleanNode *node = new (bufferNode) BooleanNode(value);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<OBoolean &>(h);
}

}  // namespace Poincare
