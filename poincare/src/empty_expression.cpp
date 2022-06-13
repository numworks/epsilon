#include <poincare/empty_expression.h>
#include <poincare/complex.h>
#include <poincare/empty_layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

int EmptyExpressionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::CodePoint(buffer, bufferSize, UCodePointEmpty);
}

Layout EmptyExpressionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return EmptyLayout::Builder();
}

template<typename T> Evaluation<T> EmptyExpressionNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return Complex<T>::Undefined();
}

}
