#ifndef POINCARE_EXPRESSION_OR_FLOAT_H
#define POINCARE_EXPRESSION_OR_FLOAT_H

#include <span>

#include "poincare/expression.h"
#include "poincare/print_float.h"

namespace Poincare {

/* Common interface for Poincare::Expression and float values. It is possible to
 * use this class in an environment that forbids exact calculations (for
 * instance in the Python app where the TreeStack is not available), but on the
 * condition that only the float variant of ExpressionOrFloat is used. */
class ExpressionOrFloat {
 public:
  ExpressionOrFloat() = default;

  constexpr static size_t k_numberOfSignificantDigits =
      PrintFloat::k_floatNumberOfSignificantDigits;
  constexpr static size_t k_bufferLength =
      PrintFloat::charSizeForFloatsWithPrecision(k_numberOfSignificantDigits);

  // TODO: fine-tune, check that it complies with the spec
  constexpr static size_t k_maxExactSerializationLength = 9;

  explicit ExpressionOrFloat(Expression expression) {
    [[maybe_unused]] size_t usedLength = expression.serialize(
        m_buffer, k_bufferLength, true, Preferences::PrintFloatMode::Decimal,
        k_numberOfSignificantDigits);
    assert(usedLength <= k_bufferLength);
  }

  explicit ExpressionOrFloat(float value) : m_value(value) {}

  explicit operator float() const { return approximation<float>(); }
  explicit operator double() const { return approximation<double>(); }

  bool hasNoExactExpression() const { return m_buffer[0] == '\0'; }

  /* Writes the expression or float representation into the provided buffer.
   * - If the instance does not contain an exact expression, write the stored
   * floating-point value.
   * - If the expression can be represented exactly by a decimal number
   * (example: 2/5 = 0.4), the decimal form (0.4) will be written.
   * - If the expression is not a decimal and its exact representation is
   * smaller than k_maxExactSerializationLength (example: 2/3), this exact
   * representation is written.
   * - If the exact representation takes more characters than the above limit
   * (example: 12/721), the approximation is written in decimal format
   * (0.016644).
   * The text lengths of what was written are returned.
   */
  PrintFloat::TextLengths writeText(
      std::span<char> buffer,
      int numberOfSignificantDigits = k_numberOfSignificantDigits,
      Preferences::PrintFloatMode floatDisplayMode =
          Preferences::PrintFloatMode::Decimal) const;

  Expression expression() const {
    if (hasNoExactExpression()) {
      return Expression::Builder(m_value);
    }
    return Expression::Parse(m_buffer, nullptr);
  }

  template <typename T>
  T approximation() const {
    return hasNoExactExpression() ? static_cast<T>(m_value)
                                  : expression().approximateToRealScalar<T>();
  }

  bool operator==(const ExpressionOrFloat& other) const {
    return hasNoExactExpression() ? (m_value == other.m_value)
                                  : (strcmp(m_buffer, other.m_buffer) == 0);
  }

 private:
  /* m_buffer is an internal way of storing an Expression. It is convenient
   * because the Pool is not preserved when the current App is closed. It could
   * be replaced by a pointer to a Tree in a preserved location, for example the
   * Storage. */
  char m_buffer[k_bufferLength] = "";
  float m_value = NAN;
};

}  // namespace Poincare

#endif
