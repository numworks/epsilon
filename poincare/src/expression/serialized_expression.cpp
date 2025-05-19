#include <poincare/serialized_expression.h>

#include "poincare/helpers/expression_equal_sign.h"
#include "poincare/print_float.h"
#include "projection.h"

namespace Poincare {

void SerializeFloatValue(float value, std::span<char> buffer,
                         int numberOfSignificantDigits,
                         Preferences::PrintFloatMode floatDisplayMode) {
  [[maybe_unused]] PrintFloat::TextLengths floatSerializationLengths =
      PrintFloat::ConvertFloatToText(
          value, buffer.data(), buffer.size(),
          Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
              numberOfSignificantDigits),
          numberOfSignificantDigits, floatDisplayMode);
  assert(floatSerializationLengths.CharLength <= buffer.size());
}

void SerializedExpression::writeText(
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
      return;
    }
  }
  return SerializeFloatValue(approximate, buffer, numberOfSignificantDigits,
                             floatDisplayMode);
}

}  // namespace Poincare
