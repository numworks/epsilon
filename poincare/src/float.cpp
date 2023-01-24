#include <poincare/float.h>
#include <poincare/layout_helper.h>

namespace Poincare {

template<typename T>
int FloatNode<T>::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert((e->type() == ExpressionNode::Type::Float && sizeof(T) == sizeof(float)) || (e->type() == ExpressionNode::Type::Double && sizeof(T) == sizeof(double)));
  const FloatNode<T> * other = static_cast<const FloatNode<T> *>(e);
  if (value() < other->value()) {
    return -1;
  }
  if (value() > other->value()) {
    return 1;
  }
  return 0;
}

template<typename T>
int FloatNode<T>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return PrintFloat::ConvertFloatToText(m_value, buffer, bufferSize, PrintFloat::k_maxFloatGlyphLength, numberOfSignificantDigits, floatDisplayMode).CharLength;
}

template<typename T>
Layout FloatNode<T>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  char buffer[PrintFloat::k_maxFloatCharSize];
  int numberOfChars = serialize(buffer, PrintFloat::k_maxFloatCharSize, floatDisplayMode, numberOfSignificantDigits);
  return LayoutHelper::String(buffer, numberOfChars);
}

template<typename T>
Float<T> Float<T>::Builder(T value) {
  void * bufferNode = TreePool::sharedPool->alloc(sizeof(FloatNode<T>));
  FloatNode<T> * node = new (bufferNode) FloatNode<T>(value);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<Float &>(h);
}

template class FloatNode<float>;
template class FloatNode<double>;

template Float<float> Float<float>::Builder(float value);
template Float<double> Float<double>::Builder(double value);

}
