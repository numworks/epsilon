#include <ion/unicode/utf8_decoder.h>
#include <poincare/old/layout.h>
#include <poincare/old/rightwards_arrow_expression.h>
#include <poincare/old/serialization_helper.h>

#include <utility>

namespace Poincare {

size_t RightwardsArrowExpressionNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  constexpr size_t stringMaxSize = CodePoint::MaxCodePointCharLength + 1;
  char string[stringMaxSize];
  SerializationHelper::CodePoint(string, stringMaxSize,
                                 UCodePointRightwardsArrow);
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode,
                                    numberOfSignificantDigits, string);
}

}  // namespace Poincare
