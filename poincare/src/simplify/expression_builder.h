#ifndef POINCARE_SIMPLIFY_EXPRESSION_BUILDER_H
#define POINCARE_SIMPLIFY_EXPRESSION_BUILDER_H

#include <poincare/expression.h>
#include "expression_match.h"
extern "C" {
#include <stdint.h>
}


class ExpressionBuilder {
  typedef Expression * (ExternalGenerator)(Expression ** parameters, int numberOfParameters);
public:
  ExpressionBuilder * child(int i);
  Expression * build(ExpressionMatch matches[]);

  enum class Action {
    BuildFromTypeAndValue,
    Clone,
    BringUpWildcard,
    CallExternalGenerator
  };

  Action m_action;

  union {
    // m_action == BuildFromTypeAndValue
    struct {
      Expression::Type m_expressionType;
      union {
        // m_expressionType == Integer
        int32_t m_integerValue;
        // m_expressionType == Symbol
        char const * m_symbolName;
      };
    };
    // m_action == Clone or BringUpWildcard
    uint8_t m_matchIndex;
    // m_action == CallExternalGenerator
    ExternalGenerator * m_generator;
  };

  uint8_t m_numberOfChildren;
};

#endif
