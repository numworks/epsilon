#ifndef POINCARE_SIMPLIFY_EXPRESSION_BUILDER_H
#define POINCARE_SIMPLIFY_EXPRESSION_BUILDER_H

#include <poincare/expression.h>
extern "C" {
#include <stdint.h>
}

class ExpressionBuilder {
public:
  ExpressionBuilder * child(int i);
  Expression * build(Expression * matches[]);

  /*Expression ** match(Expression * e);*/

  enum class Action {
    Build,
    Clone,
    FunctionCall
  };

  Action m_action;

  union {
    // m_action == Build
    struct {
      Expression::Type m_expressionType;
      union {
        // m_expressionType == Integer
        int32_t m_integerValue;
        // m_expressionType == Symbol
        char const * m_symbolName;
      };
    };
    // m_action == Clone
    uint8_t m_matchIndex;
    // m_action == FunctionCall
    void * m_functionPointer;
  };

  uint8_t m_numberOfChildren;
};

#endif
