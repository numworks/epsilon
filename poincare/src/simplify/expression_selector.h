#ifndef POINCARE_SIMPLIFY_EXPRESSION_SELECTOR_H
#define POINCARE_SIMPLIFY_EXPRESSION_SELECTOR_H

#include <poincare/expression.h>
#include "expression_match.h"
extern "C" {
#include <stdint.h>
}

class ExpressionSelector {
public:
  static constexpr ExpressionSelector Any(uint8_t numberOfChildren);
  static constexpr ExpressionSelector Wildcard(uint8_t numberOfChildren);
  static constexpr ExpressionSelector Type(Expression::Type type, uint8_t numberOfChildren);
  static constexpr ExpressionSelector TypeAndValue(Expression::Type type, int32_t value, uint8_t numberOfChildren);

  /* The match function is interesting
   * - It returns 0 if the selector didn't match the expression
   * - Otherwise, it returns the number of captured matches and sets *matches
   * Caution: This function *will* write to *matches even if the returned
   * value is zero.
   */
  int match(const Expression * e, ExpressionMatch * matches);
private:
  enum class Match {
    Any,
    Type,
    Wildcard,
    TypeAndValue,
  };

  constexpr ExpressionSelector(Match match, Expression::Type type, int32_t integerValue, uint8_t numberOfChildren);

  int numberOfNonWildcardChildren();
  bool canCommutativelyMatch(const Expression * e, ExpressionMatch * matches,
      uint8_t * selectorMatched, int leftToMatch);
  int commutativeMatch(const Expression * e, ExpressionMatch * matches);
  int sequentialMatch(const Expression * e, ExpressionMatch * matches);
  ExpressionSelector * child(int index);

  Match m_match;
  union {
    // m_match == Any
    // nothing!
    // m_match == ExpressionType
    struct {
      Expression::Type m_expressionType;
      union {
        // m_expressionType == Integer
        int32_t m_integerValue;
        // m_expressionType == Symbol
        char * m_symbolName;
      };
    };
  };
  uint8_t m_numberOfChildren;
};

/* Since they have to be evaluated at compile time, constexpr functions are
 * implicitely defined inline. Therefore we have to provide their implementation
 * in this header. */

constexpr ExpressionSelector ExpressionSelector::Any(uint8_t numberOfChildren) {
  return ExpressionSelector(Match::Any, (Expression::Type)0, 0, numberOfChildren);
}

constexpr ExpressionSelector ExpressionSelector::Wildcard(uint8_t numberOfChildren) {
  return ExpressionSelector(Match::Wildcard, (Expression::Type)0, 0, numberOfChildren);
}

constexpr ExpressionSelector ExpressionSelector::Type(Expression::Type type, uint8_t numberOfChildren) {
  return ExpressionSelector(Match::Type, type, 0, numberOfChildren);
}

constexpr ExpressionSelector ExpressionSelector::TypeAndValue(Expression::Type type, int32_t value, uint8_t numberOfChildren) {
  return ExpressionSelector(Match::TypeAndValue, type, value, numberOfChildren);
}

constexpr ExpressionSelector::ExpressionSelector(Match match,
  Expression::Type type,
  int32_t integerValue,
  uint8_t numberOfChildren)
  :
  m_match(match),
  m_expressionType(type),
  m_integerValue(integerValue),
  m_numberOfChildren(numberOfChildren) {
}

#endif
