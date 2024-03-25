#include <poincare/complex.h>
#include <poincare/empty_expression.h>
#include <poincare/layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

size_t EmptyExpressionNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::CodePoint(buffer, bufferSize, UCodePointEmpty);
}

template <typename T>
Evaluation<T> EmptyExpressionNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  return Complex<T>::Undefined();
}

}  // namespace Poincare
