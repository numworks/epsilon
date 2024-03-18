#include <poincare/float.h>
#include <poincare/layout_helper.h>
#include <poincare_junior/src/memory/edition_pool.h>

namespace Poincare {

template <typename T>
int FloatNode<T>::simplificationOrderSameType(const ExpressionNode *e,
                                              bool ascending,
                                              bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert((e->otype() == ExpressionNode::Type::Float &&
          sizeof(T) == sizeof(float)) ||
         (e->otype() == ExpressionNode::Type::Double &&
          sizeof(T) == sizeof(double)));
  const FloatNode<T> *other = static_cast<const FloatNode<T> *>(e);
  if (value() < other->value()) {
    return -1;
  }
  if (value() > other->value()) {
    return 1;
  }
  return 0;
}

template <typename T>
size_t FloatNode<T>::serialize(char *buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  return PrintFloat::ConvertFloatToText(
             m_value, buffer, bufferSize, PrintFloat::k_maxFloatGlyphLength,
             numberOfSignificantDigits, floatDisplayMode)
      .CharLength;
}

template <typename T>
Float<T> Float<T>::Builder(T value) {
  void *bufferNode = TreePool::sharedPool->alloc(sizeof(FloatNode<T>));
  FloatNode<T> *node = new (bufferNode) FloatNode<T>(value);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<Float &>(h);
}

template <>
JuniorFloat<float> JuniorFloat<float>::Builder(float value) {
  JuniorExpression expr = JuniorExpression::Builder(
      PoincareJ::SharedEditionPool->push<PoincareJ::BlockType::SingleFloat>(
          value));
  return static_cast<JuniorFloat<float> &>(expr);
}

template <>
JuniorFloat<double> JuniorFloat<double>::Builder(double value) {
  JuniorExpression expr = JuniorExpression::Builder(
      PoincareJ::SharedEditionPool->push<PoincareJ::BlockType::DoubleFloat>(
          value));
  return static_cast<JuniorFloat<double> &>(expr);
}

template class FloatNode<float>;
template class FloatNode<double>;

template Float<float> Float<float>::Builder(float value);
template Float<double> Float<double>::Builder(double value);

}  // namespace Poincare
