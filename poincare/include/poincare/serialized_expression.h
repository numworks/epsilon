#ifndef POINCARE_SERIALIZED_EXPRESSION_H
#define POINCARE_SERIALIZED_EXPRESSION_H

#include "expression.h"

namespace Poincare {

template <size_t MaxLength>
class SerializedExpression {
 public:
  SerializedExpression() = default;

  explicit SerializedExpression(Expression expression) {
    [[maybe_unused]] size_t usedLength = expression.serialize(
        m_buffer, MaxLength, Preferences::PrintFloatMode::Decimal,
        PrintFloat::k_maxNumberOfSignificantDigits);
    assert(usedLength <= MaxLength);
  }

  explicit SerializedExpression(float value)
      : SerializedExpression(Poincare::Expression::Builder(value)) {}

  explicit operator float() const {
    return isUninitialized()
               ? NAN
               : expression().template approximateToRealScalar<float>();
  }

  bool isUninitialized() const { return m_buffer[0] == '\0'; }

  const char* text() const { return m_buffer; }

  Expression expression() const {
    assert(!isUninitialized());
    return Expression::Parse(m_buffer, nullptr);
  }

  bool operator==(const SerializedExpression& other) const {
    return strcmp(m_buffer, other.m_buffer) == 0;
  }

 private:
  char m_buffer[MaxLength] = "";
};

}  // namespace Poincare

#endif
