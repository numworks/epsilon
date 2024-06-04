#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/empty_expression.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

size_t EmptyExpressionNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::CodePoint(buffer, bufferSize, UCodePointEmpty);
}

}  // namespace Poincare
