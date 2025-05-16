#ifndef POINCARE_SERIALIZED_EXPRESSION_H
#define POINCARE_SERIALIZED_EXPRESSION_H

#include <span>

#include "poincare/expression.h"

namespace Poincare {

class SerializedExpression {
 public:
  SerializedExpression() = default;

  constexpr static size_t k_numberOfSignificantDigits =
      PrintFloat::k_floatNumberOfSignificantDigits;
  constexpr static size_t k_bufferLength =
      PrintFloat::charSizeForFloatsWithPrecision(k_numberOfSignificantDigits);

  explicit SerializedExpression(Expression expression) {
    [[maybe_unused]] size_t usedLength = expression.serialize(
        m_buffer, k_bufferLength, Preferences::PrintFloatMode::Decimal,
        k_numberOfSignificantDigits);
    assert(usedLength <= k_bufferLength);
  }

  explicit SerializedExpression(float value)
      : SerializedExpression(Poincare::Expression::Builder(value)) {}

  explicit operator float() const {
    return isUninitialized()
               ? NAN
               : expression().template approximateToRealScalar<float>();
  }

  bool isUninitialized() const { return m_buffer[0] == '\0'; }

  void writeText(std::span<char> buffer,
                 int numberOfSignificantDigits = k_numberOfSignificantDigits,
                 Preferences::PrintFloatMode floatDisplayMode =
                     Preferences::PrintFloatMode::Decimal) const;

  Expression expression() const {
    assert(!isUninitialized());
    return Expression::Parse(m_buffer, nullptr);
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
