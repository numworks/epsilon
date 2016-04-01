#ifndef POINCARE_SIMPLIFY_EXPRESSION_SELECTOR_H
#define POINCARE_SIMPLIFY_EXPRESSION_SELECTOR_H

#include <poincare/expression.h>
#include "expression_match.h"
extern "C" {
#include <stdint.h>
}

class ExpressionSelector {
  /* Everything must be made public otherwise we cannot static-initialize an
   * array of ExpressionSelectors. */
public:
  enum class Match {
    Any,
    Type,
    Wildcard,
    TypeAndValue,
  };
  ExpressionSelector * child(int i);
  /* The match function is interesting
   * - It returns 0 if the selector didn't match the expression
   * - Otherwise, it returns the number of captured matches and sets *matches
   * Caution: This function *will* write to *matches even if the returned
   * value is zero.
   */
  int match(Expression * e, ExpressionMatch * matches);

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
  uint8_t m_numberOfChildren;
};

#endif
