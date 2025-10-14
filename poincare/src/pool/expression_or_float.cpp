#include "poincare/expression_or_float.h"

#include <poincare/helpers/layout.h>

#include "omg/utf8_helper.h"
#include "poincare/helpers/expression_equal_sign.h"
#include "poincare/print_float.h"

namespace Poincare {

PrintFloat::TextLengths SerializeFloatValue(
    float value, std::span<char> buffer, size_t numberOfSignificantDigits,
    Preferences::PrintFloatMode floatDisplayMode, size_t maxGlyphLength) {
  PrintFloat::TextLengths floatSerializationLengths =
      PrintFloat::ConvertFloatToText(value, buffer.data(), buffer.size(),
                                     maxGlyphLength, numberOfSignificantDigits,
                                     floatDisplayMode);
  /*  PrintFloat::ConvertFloatToText can return the "exception result"
   * {.CharLength = bufferSize, .GlyphLength = maxGlyphLength + 1} if it was not
   * possible to write into the buffer with the requested parameters. In the
   * ExpressionOrFloat context, it is better to return zero lengths to match
   * with the written buffer length (which is zero as nothing was written by
   * PrintFloat::ConvertFloatToText). */
  if (floatSerializationLengths ==
      PrintFloat::TextLengths{buffer.size(), maxGlyphLength + 1}) {
    assert(buffer[0] == '\0');
    return PrintFloat::TextLengths{0, 0};
  }
  assert(floatSerializationLengths.CharLength <= buffer.size());
  assert(floatSerializationLengths.CharLength == strlen(buffer.data()));
  return floatSerializationLengths;
}

PrintFloat::TextLengths SerializeExactExpression(
    UserExpression expression, std::span<char> buffer,
    size_t numberOfSignificantDigits,
    Preferences::PrintFloatMode floatDisplayMode) {
  size_t exactStringLength =
      expression.serialize(buffer, true, floatDisplayMode,
                           static_cast<int>(numberOfSignificantDigits));
  /* Serialization may fail if it does not hold on the buffer, in that case an
   * error code is returned as the string length. The approximate expression
   * will be used instead. */
  size_t exactGlyphLength = UTF8Helper::StringGlyphLength(buffer.data());
  return PrintFloat::TextLengths{exactStringLength, exactGlyphLength};
}

PrintFloat::TextLengths ExpressionOrFloat::writeText(
    std::span<char> buffer, ApproximationParameters approximationParameters,
    size_t numberOfSignificantDigits,
    Preferences::PrintFloatMode floatDisplayMode, size_t maxGlyphLength) const {
  if (hasNoExactExpression()) {
    return SerializeFloatValue(m_value, buffer, numberOfSignificantDigits,
                               floatDisplayMode, maxGlyphLength);
  }
  UserExpression exactExpression = expression();
  float approximation =
      Approximate<float>(exactExpression, approximationParameters);
  if (!IsCalculationOutputStrictEquality(exactExpression,
                                         UserExpression::Builder(approximation),
                                         numberOfSignificantDigits)) {
    constexpr size_t k_bufferExactLength = 15;
    char exactSerialization[k_bufferExactLength];
    PrintFloat::TextLengths exactTextLengths =
        SerializeExactExpression(exactExpression, exactSerialization,
                                 numberOfSignificantDigits, floatDisplayMode);
    bool firstCharIsMinus = exactSerialization[0] == '-';
    /* NOTE: minus sign size is ignored to avoid displaying exact values on the
     * positive part of the axis and approximation on the negative part */
    if ((exactTextLengths.GlyphLength <=
         k_maxExactSerializationGlyphLength + (firstCharIsMinus ? 1 : 0)) &&
        (exactTextLengths.GlyphLength <= maxGlyphLength) &&
        (exactTextLengths.CharLength <= k_bufferExactLength) &&
        ((exactTextLengths.CharLength <= buffer.size()))) {
      assert(exactTextLengths.CharLength !=
             Poincare::LayoutHelpers::k_bufferOverflow);
      strlcpy(buffer.data(), exactSerialization,
              exactTextLengths.CharLength + 1);
      return exactTextLengths;
    }
  }
  return SerializeFloatValue(approximation, buffer, numberOfSignificantDigits,
                             floatDisplayMode, maxGlyphLength);
}

UserExpression ExpressionOrFloat::expression() const {
  if (hasNoExactExpression()) {
    return UserExpression::Builder(m_value);
  }
  return UserExpression::Builder(Internal::Tree::FromBlocks(m_buffer.data()));
}

bool ExpressionOrFloat::operator==(const ExpressionOrFloat& other) const {
  if (hasNoExactExpression() != other.hasNoExactExpression()) {
    return false;
  }
  if (hasNoExactExpression()) {
    return (m_value == other.m_value);
  }
  const Internal::Tree* tree = Internal::Tree::FromBlocks(m_buffer.data());
  const Internal::Tree* otherTree =
      Internal::Tree::FromBlocks(other.m_buffer.data());
  return tree->treeIsIdenticalTo(otherTree);
}

bool ExpressionOrFloat::ExpressionFitsBuffer(UserExpression expression) {
  /* We ignore the cost of the [Opposite] node when storing the tree in the
   * buffer:
   * Either the expression is negative and smaller than [k_maxTreeSize].
   * Or it is positive and smaller than [k_maxTreeSize - 1]. */
  static_assert(k_oppositeNodeSize ==
                Internal::TypeBlock(Internal::Type::Opposite).nodeSize());
  size_t treeSize = expression.tree()->treeSize();
  treeSize -= expression.tree()->isOpposite() ? k_oppositeNodeSize : 0;
  return treeSize <= k_maxTreeSize;
}

}  // namespace Poincare
