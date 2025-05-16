#include <poincare/serialized_expression.h>

#include "poincare/helpers/expression_equal_sign.h"
#include "poincare/print_float.h"
#include "projection.h"

namespace Poincare {

void SerializedExpression::writeText(
    std::span<char> buffer, int numberOfSignificantDigits,
    Preferences::PrintFloatMode floatDisplayMode) const {
  /*  Note: m_buffer is just an internal storage, but it does not have the
   * requested number of significant digits or display mode. It should thus not
   * be returned directly. The expression is re-constructed, then serialized
   * with the requested display parameters. */
  UserExpression exactExpression = expression();
  double approximate = exactExpression.approximateToRealScalar<double>();
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
  [[maybe_unused]] PrintFloat::TextLengths approximationSerializationLengths =
      PrintFloat::ConvertFloatToText(
          approximate, buffer.data(), buffer.size(),
          Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
              numberOfSignificantDigits),
          numberOfSignificantDigits, floatDisplayMode);
  assert(approximationSerializationLengths.CharLength <= buffer.size());
}

}  // namespace Poincare
