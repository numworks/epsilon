#ifndef POINCARE_EXPRESSION_OR_FLOAT_H
#define POINCARE_EXPRESSION_OR_FLOAT_H

#include <poincare/expression.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/src/memory/tree.h>

#include <span>

namespace Poincare {

/* Common interface for Poincare::Expression and float values. It is possible to
 * use this class in an environment that forbids exact calculations (for
 * instance in the Python app where the TreeStack is not available), but on the
 * condition that only the float variant of ExpressionOrFloat is used. */
class ExpressionOrFloat {
 public:
  ExpressionOrFloat() = default;
  explicit ExpressionOrFloat(float value) : m_value(value) {}

  using ApproximationFunction = float (*)(Expression);

  /*  Create an ExpressionOrFloat from an Expression. The input Expression may
   * be uninitialized or too big to hold on the internal storage of
   * ExpressionOrFloat. The only requirement is that the input expression, if
   * initialized, should represent a scalar value. */
  static ExpressionOrFloat Builder(
      Expression expression, ApproximationFunction approximationFunction) {
    if (expression.isUninitialized()) {
      return ExpressionOrFloat();
    }
    assert(Poincare::Dimension(expression).isScalar());
    if (ExpressionFitsBuffer(expression)) {
      return ExpressionOrFloat(expression);
    }
    // Fallback on storing the approximation if the expression is too large
    return ExpressionOrFloat(approximationFunction(expression));
  }

  bool hasNoExactExpression() const { return m_buffer[0] == '\0'; }

  struct ApproximationParameters {
    Preferences::AngleUnit angleUnit;
    Preferences::ComplexFormat complexFormat;
  };

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
      std::span<char> buffer, ApproximationParameters approximationParameters,
      size_t numberOfSignificantDigits,
      Preferences::PrintFloatMode floatDisplayMode,
      size_t maxGlyphLength) const;
  PrintFloat::TextLengths writeText(
      std::span<char> buffer, ApproximationParameters approximationParameters,
      size_t numberOfSignificantDigits = k_numberOfSignificantDigits,
      Preferences::PrintFloatMode floatDisplayMode =
          Preferences::PrintFloatMode::Decimal) const {
    return writeText(buffer, approximationParameters, numberOfSignificantDigits,
                     floatDisplayMode, buffer.size());
  }

  Expression expression() const {
    if (hasNoExactExpression()) {
      return Expression::Builder(m_value);
    }
    return Expression::Builder(Internal::Tree::FromBlocks(m_buffer.data()));
  }

  template <typename T>
  T approximation(ApproximationParameters approximationParameters) const {
    return hasNoExactExpression()
               ? static_cast<T>(m_value)
               : Approximate<T>(expression(), approximationParameters);
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
  constexpr static size_t k_oppositeNodeSize =
      Internal::TypeBlock(Internal::Type::Opposite).nodeSize();
  /* Max tree size, including potential [Opposite] node */
  constexpr static size_t k_maxTreeSize = 8 + k_oppositeNodeSize;
  constexpr static size_t k_numberOfSignificantDigits =
      PrintFloat::k_floatNumberOfSignificantDigits;

  // This max ignores the "-" sign, the true maximum is one more than this value
  constexpr static size_t k_maxExactSerializationGlyphLength = 5;

  /* The constructor from an expression is private. The Builder method should be
   * used when creating an ExpressionOrFloat from an expression. */
  explicit ExpressionOrFloat(Expression expression) {
    assert(!expression.isUninitialized());
    assert(Poincare::Dimension(expression).isScalar());
    assert(ExpressionFitsBuffer(expression));
    expression.tree()->copyTreeTo(m_buffer.data());
  }

  /* The approximation parameters are fixed to Radian and Real in the context of
   * ExpressionOrFloat. */
  template <typename T>
  static T Approximate(UserExpression expression,
                       ApproximationParameters approximationParameters) {
    assert(!expression.isUninitialized() &&
           Poincare::Dimension(expression).isScalar());
    return expression.approximateToRealScalar<T>(
        approximationParameters.angleUnit,
        approximationParameters.complexFormat);
  }

  /* We ignore the cost of the [Opposite] node when storing the tree in the
   * buffer:
   * Either the expression is negative and smaller than [k_maxTreeSize].
   * Or it is positive and smaller than [k_maxTreeSize - 1]. */
  static bool ExpressionFitsBuffer(Expression expression) {
    size_t treeSize = expression.tree()->treeSize();
    treeSize -= expression.tree()->isOpposite() ? k_oppositeNodeSize : 0;
    return treeSize <= k_maxTreeSize;
  }

  /* The Pool (where Expressions are stored) is not preserved when the current
   * App is closed. So for the expression to be preserved when closing and
   * reopening the App, ExpressionOrFloat needs to store the expression Tree in
   * a buffer of Blocks. */
  std::array<Internal::Block, k_maxTreeSize> m_buffer;
  float m_value = NAN;
};

}  // namespace Poincare

#endif
