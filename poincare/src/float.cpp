#include <poincare/float.h>
#include <poincare/layout_helper.h>

namespace Poincare {

template<typename T>
Expression FloatNode<T>::setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  assert(s != Sign::Unknown);
  Sign currentSign = m_value < 0 ? Sign::Negative : Sign::Positive;
  Expression thisExpr = Number(this);
  Expression result = Float<T>(s == currentSign ? m_value : -m_value);
  thisExpr.replaceWithInPlace(result);
  return result;
}

template<typename T>
int FloatNode<T>::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, canBeInterrupted);
  }
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

template<typename T>
int FloatNode<T>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return PrintFloat::convertFloatToText(m_value, buffer, bufferSize, numberOfSignificantDigits, floatDisplayMode);
}

template<typename T>
Layout FloatNode<T>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  char buffer[PrintFloat::k_maxFloatBufferLength];
  int numberOfChars = serialize(buffer, PrintFloat::k_maxFloatBufferLength, floatDisplayMode, numberOfSignificantDigits);
  return LayoutHelper::String(buffer, numberOfChars);
}

template<typename T>
Float<T>::Float(T value) : Number(TreePool::sharedPool()->createTreeNode<FloatNode<T>>()) {
  node()->setFloat(value);
}

template class FloatNode<float>;
template class FloatNode<double>;

template Float<float>::Float(float value);
template Float<double>::Float(double value);

}
