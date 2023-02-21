#include <poincare/boolean.h>
#include <poincare/layout_helper.h>

namespace Poincare {

template <typename T>
Expression BooleanEvaluationNode<T>::complexToExpression(
    Preferences::Preferences::ComplexFormat complexFormat) const {
  return Boolean::Builder(value());
}

template <typename T>
BooleanEvaluation<T> BooleanEvaluation<T>::Builder(bool value) {
  void *bufferNode =
      TreePool::sharedPool->alloc(sizeof(BooleanEvaluationNode<T>));
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

Layout BooleanNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits,
                                 Context *context) const {
  return LayoutHelper::String(aliasesList().mainAlias());
}

int BooleanNode::serialize(char *buffer, int bufferSize,
                           Preferences::PrintFloatMode floatDisplayMode,
                           int numberOfSignificantDigits) const {
  return strlcpy(buffer, aliasesList().mainAlias(), bufferSize);
}

Boolean Boolean::Builder(bool value) {
  void *bufferNode = TreePool::sharedPool->alloc(sizeof(BooleanNode));
  BooleanNode *node = new (bufferNode) BooleanNode(value);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<Boolean &>(h);
}

}  // namespace Poincare