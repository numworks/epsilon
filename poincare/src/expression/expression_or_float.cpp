#include "poincare/expression_or_float.h"

#include "omg/utf8_helper.h"
#include "poincare/helpers/expression_equal_sign.h"
#include "poincare/print_float.h"
#include "projection.h"

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
    Expression expression, std::span<char> buffer,
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
  if (!ExactAndApproximateExpressionsAreStrictlyEqual(
          exactExpression, UserExpression::Builder(approximation))) {
    constexpr size_t k_bufferExactLength = 15;
    char exactSerialization[k_bufferExactLength];
    PrintFloat::TextLengths exactTextLengths =
        SerializeExactExpression(exactExpression, exactSerialization,
                                 numberOfSignificantDigits, floatDisplayMode);
    bool firstCharIsMinus = exactSerialization[0] == '-';
    /* NOTE: minus sign size is ignored to avoid displaying exact values on the
     * positive part of the axis and approximation on the negative part */
    if ((exactTextLengths.GlyphLength - (firstCharIsMinus ? 1 : 0) <=
         k_maxExactSerializationGlyphLength) &&
        (exactTextLengths.GlyphLength <= maxGlyphLength) &&
        (exactTextLengths.CharLength <= k_bufferExactLength) &&
        ((exactTextLengths.CharLength <= buffer.size()))) {
      strlcpy(buffer.data(), exactSerialization,
              exactTextLengths.CharLength + 1);
      return exactTextLengths;
    }
  }
  return SerializeFloatValue(approximation, buffer, numberOfSignificantDigits,
                             floatDisplayMode, maxGlyphLength);
}

}  // namespace Poincare
