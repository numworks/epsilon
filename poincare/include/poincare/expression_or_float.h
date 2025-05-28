#ifndef POINCARE_EXPRESSION_OR_FLOAT_H
#define POINCARE_EXPRESSION_OR_FLOAT_H

#include <span>

#include "poincare/expression.h"
#include "poincare/preferences.h"
#include "poincare/print_float.h"
#include "poincare/src/memory/tree.h"

namespace Poincare {

/* Common interface for Poincare::Expression and float values. It is possible to
 * use this class in an environment that forbids exact calculations (for
 * instance in the Python app where the TreeStack is not available), but on the
 * condition that only the float variant of ExpressionOrFloat is used. */
class ExpressionOrFloat {
 public:
  ExpressionOrFloat() = default;

  constexpr static size_t k_maxExpressionSize = 20;

  constexpr static size_t k_numberOfSignificantDigits =
      PrintFloat::k_floatNumberOfSignificantDigits;

  constexpr static size_t k_maxExactSerializationGlyphLength = 5;

  explicit ExpressionOrFloat(Expression expression) {
    if (!expression.isUninitialized()) {
      /*  TODO: ensure on the caller side that the passed expression is not
       * bigger than than k_maxExpressionSize */
      assert(expression.tree()->treeSize() <= k_maxExpressionSize);
      expression.tree()->copyTreeTo(m_buffer.data());
    }
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
   * smaller than k_maxExactSerializationGlyphLength (example: 2/3), this exact
   * representation is written.
   * - If the exact representation takes more characters than the above limit
   * (example: 12/721), the approximation is written in decimal format
   * (0.016644).
   * The text lengths of what was written are returned. */
  /* Note: the contained expression should be a UserExpression when using the
   * writeText method to display the expression to the user. */

  PrintFloat::TextLengths writeText(
      std::span<char> buffer, size_t numberOfSignificantDigits,
      Preferences::PrintFloatMode floatDisplayMode,
      size_t maxGlyphLength) const;
  PrintFloat::TextLengths writeText(
      std::span<char> buffer,
      size_t numberOfSignificantDigits = k_numberOfSignificantDigits,
      Preferences::PrintFloatMode floatDisplayMode =
          Preferences::PrintFloatMode::Decimal) const {
    return writeText(buffer, numberOfSignificantDigits, floatDisplayMode,
                     buffer.size());
  }

  Expression expression() const {
    if (hasNoExactExpression()) {
      return Expression::Builder(m_value);
    }
    return Expression::Builder(Internal::Tree::FromBlocks(m_buffer.data()));
  }

  template <typename T>
  T approximation() const {
    return hasNoExactExpression() ? static_cast<T>(m_value)
                                  : approximate<T>(expression());
  }

  bool operator==(const ExpressionOrFloat& other) const {
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

 private:
  /* The approximation parameters are fixed to Radian and Real in the context of
   * ExpressionOrFloat. */
  template <typename T>
  static T approximate(UserExpression expression) {
    return expression.approximateToRealScalar<T>(
        Preferences::AngleUnit::Radian, Preferences::ComplexFormat::Real);
  }

  /* The Pool (where Expressions are stored) is not preserved when the current
   * App is closed. So for the expression to be preserved when closing and
   * reopening the App, ExpressionOrFloat needs to store the expression Tree in
   * a buffer of Blocks. */
  std::array<Internal::Block, k_maxExpressionSize> m_buffer;
  float m_value = NAN;
};

}  // namespace Poincare

#endif
