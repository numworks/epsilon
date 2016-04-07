#ifndef POINCARE_SIMPLIFY_EXPRESSION_SELECTOR_H
#define POINCARE_SIMPLIFY_EXPRESSION_SELECTOR_H

#include <poincare/expression.h>
#include "contiguous_tree.h"
#include "expression_match.h"
extern "C" {
#include <stdint.h>
}

class ExpressionSelector : public ContiguousTree {
public:
  static constexpr ExpressionSelector Any(uint8_t numberOfChildren) {
    return ExpressionSelector(Match::Any, (Expression::Type)0, 0, numberOfChildren);
  }
  static constexpr ExpressionSelector Wildcard(uint8_t numberOfChildren) {
    return ExpressionSelector(Match::Wildcard, (Expression::Type)0, 0, numberOfChildren);
  }
  static constexpr ExpressionSelector Type(Expression::Type type, uint8_t numberOfChildren) {
    return ExpressionSelector(Match::Type, type, 0, numberOfChildren);
  }
  static constexpr ExpressionSelector TypeAndValue(Expression::Type type, int32_t value, uint8_t numberOfChildren) {
    return ExpressionSelector(Match::TypeAndValue, type, value, numberOfChildren);
  }

  ExpressionSelector * child(int i);
  /* The match function is interesting
   * - It returns 0 if the selector didn't match the expression
   * - Otherwise, it returns the number of captured matches and sets *matches
   * Caution: This function *will* write to *matches even if the returned
   * value is zero.
   */
  int match(Expression * e, ExpressionMatch * matches);
private:
  enum class Match {
    Any,
    Type,
    Wildcard,
    TypeAndValue,
  };

  constexpr ExpressionSelector(Match match,
      Expression::Type type,
      int32_t integerValue,
      uint8_t numberOfChildren)
    :
      ContiguousTree(numberOfChildren),
      m_match(match),
      m_expressionType(type),
      m_integerValue(integerValue)
  {
  }

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
        char const * m_symbolName;
      };
    };
  };
};

#endif
