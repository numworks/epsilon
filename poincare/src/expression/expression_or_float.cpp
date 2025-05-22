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
      expression.serialize(buffer.data(), buffer.size(), true, floatDisplayMode,
                           static_cast<int>(numberOfSignificantDigits));
  size_t exactGlyphLength = UTF8Helper::StringGlyphLength(buffer.data());
  return PrintFloat::TextLengths{exactStringLength, exactGlyphLength};
}

PrintFloat::TextLengths ExpressionOrFloat::writeText(
    std::span<char> buffer, size_t numberOfSignificantDigits,
    Preferences::PrintFloatMode floatDisplayMode, size_t maxGlyphLength) const {
  if (hasNoExactExpression()) {
    return SerializeFloatValue(m_value, buffer, numberOfSignificantDigits,
                               floatDisplayMode, maxGlyphLength);
  }
  /*  Note: m_buffer is just an internal storage, but it does not have the
   * requested number of significant digits or display mode. It should thus
   * not be returned directly. The expression is re-constructed, then
   * serialized with the requested display parameters. */
  UserExpression exactExpression = expression();
  float approximate = exactExpression.approximateToRealScalar<float>();
  if (!ExactAndApproximateExpressionsAreStrictlyEqual(
          exactExpression, UserExpression::Builder(approximate))) {
    char exactSerialization[k_bufferLength];
    PrintFloat::TextLengths exactTextLengths =
        SerializeExactExpression(exactExpression, exactSerialization,
                                 numberOfSignificantDigits, floatDisplayMode);
    if ((exactTextLengths.GlyphLength <= k_maxExactSerializationGlyphLength) &&
        (exactTextLengths.GlyphLength <= maxGlyphLength) &&
        (exactTextLengths.CharLength <= buffer.size())) {
      strlcpy(buffer.data(), exactSerialization,
              exactTextLengths.CharLength + 1);
      return exactTextLengths;
    }
  }
  return SerializeFloatValue(approximate, buffer, numberOfSignificantDigits,
                             floatDisplayMode, maxGlyphLength);
}

}  // namespace Poincare
