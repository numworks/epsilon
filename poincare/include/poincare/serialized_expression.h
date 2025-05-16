#ifndef POINCARE_SERIALIZED_EXPRESSION_H
#define POINCARE_SERIALIZED_EXPRESSION_H

#include <poincare/expression.h>

#include <span>

namespace Poincare {

class SerializedExpression {
 public:
  SerializedExpression() = default;

  constexpr static size_t k_numberOfSignificantDigits =
      PrintFloat::k_floatNumberOfSignificantDigits;
  constexpr static size_t k_bufferLength =
      PrintFloat::charSizeForFloatsWithPrecision(k_numberOfSignificantDigits);

  // TODO: fine-tune, check that it complies with the spec
  constexpr static size_t k_maxExactSerializationLength = 9;

  explicit SerializedExpression(Expression expression) {
    [[maybe_unused]] size_t usedLength = expression.serialize(
        m_buffer, k_bufferLength, true, Preferences::PrintFloatMode::Decimal,
        k_numberOfSignificantDigits);
    assert(usedLength <= k_bufferLength);
  }

  explicit SerializedExpression(float value)
      : SerializedExpression(Poincare::Expression::Builder(value)) {}

  explicit operator float() const { return approximation<float>(); }
  explicit operator double() const { return approximation<double>(); }

  bool isUninitialized() const { return m_buffer[0] == '\0'; }

  /* Writes the expression representation into the provided buffer.
   * - If the expression can be represented exactly by a decimal number
   * (example: 2/5 = 0.4), the decimal form (0.4) will be written.
   * - If the expression is not a decimal and its exact representation is
   * smaller than k_maxExactSerializationLength (example: 2/3), this exact
   * representation is written.
   * - If the exact representation takes more characters than the above limit
   * (example: 12/721), the approximation is written in decimal format
   * (0.016644).
   */
  void writeText(std::span<char> buffer,
                 int numberOfSignificantDigits = k_numberOfSignificantDigits,
                 Preferences::PrintFloatMode floatDisplayMode =
                     Preferences::PrintFloatMode::Decimal) const;

  Expression expression() const {
    assert(!isUninitialized());
    return Expression::Parse(m_buffer, nullptr);
  }

  template <typename T>
  T approximation() const {
    return isUninitialized() ? NAN : expression().approximateToRealScalar<T>();
  }

  bool operator==(const SerializedExpression& other) const {
    return strcmp(m_buffer, other.m_buffer) == 0;
  }

 private:
  /* m_buffer is an internal way of storing an Expression. It is convenient
   * because the Pool is not preserved when the current App is closed. It could
   * be replaced by a pointer to a Tree in a preserved location, for example the
   * Storage. */
  char m_buffer[k_bufferLength] = "";
};

}  // namespace Poincare

#endif
