#include <poincare/empty_expression.h>
#include <poincare/complex.h>
#include <poincare/empty_layout.h>
#include <poincare/serialization_helper.h>
#include <ion/charset.h>

namespace Poincare {

int EmptyExpressionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Char(buffer, bufferSize, Ion::Charset::Empty);
}

Layout EmptyExpressionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return EmptyLayout();
}

template<typename T> Evaluation<T> EmptyExpressionNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return Complex<T>::Undefined();
}

EmptyExpression::EmptyExpression() : Expression(TreePool::sharedPool()->createTreeNode<EmptyExpressionNode>()) {}

}
