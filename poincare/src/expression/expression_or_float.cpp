#include "poincare/expression_or_float.h"

#include "omg/utf8_helper.h"
#include "poincare/helpers/expression_equal_sign.h"
#include "poincare/print_float.h"
#include "projection.h"

namespace Poincare {

PrintFloat::TextLengths SerializeFloatValue(
    float value, std::span<char> buffer, int numberOfSignificantDigits,
    Preferences::PrintFloatMode floatDisplayMode) {
  PrintFloat::TextLengths floatSerializationLengths =
      PrintFloat::ConvertFloatToText(
          value, buffer.data(), buffer.size(),
          Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
              numberOfSignificantDigits),
          numberOfSignificantDigits, floatDisplayMode);
  assert(floatSerializationLengths.CharLength <= buffer.size());
  assert(floatSerializationLengths.CharLength == strlen(buffer.data()));
  return floatSerializationLengths;
}

PrintFloat::TextLengths ExpressionOrFloat::writeText(
    std::span<char> buffer, int numberOfSignificantDigits,
    Preferences::PrintFloatMode floatDisplayMode) const {
  if (hasNoExactExpression()) {
    return SerializeFloatValue(m_value, buffer, numberOfSignificantDigits,
                               floatDisplayMode);
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
    size_t exactSerializationLength =
        exactExpression.serialize(exactSerialization, k_bufferLength, true,
                                  floatDisplayMode, numberOfSignificantDigits);
    if (exactSerializationLength <= k_maxExactSerializationLength) {
      assert(exactSerializationLength <= buffer.size());
      strlcpy(buffer.data(), exactSerialization, exactSerializationLength + 1);
      assert(exactSerializationLength == strlen(buffer.data()));
      return PrintFloat::TextLengths{
          exactSerializationLength,
          UTF8Helper::StringGlyphLength(buffer.data())};
    }
  }
  return SerializeFloatValue(approximate, buffer, numberOfSignificantDigits,
                             floatDisplayMode);
}

}  // namespace Poincare
