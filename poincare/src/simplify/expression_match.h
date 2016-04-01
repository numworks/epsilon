#ifndef POINCARE_SIMPLIFY_EXPRESSION_MATCH_H
#define POINCARE_SIMPLIFY_EXPRESSION_MATCH_H

#include <poincare/expression.h>
extern "C" {
#include <stdint.h>
}

class ExpressionMatch {
public:
  ExpressionMatch();
  ExpressionMatch(Expression ** expressions, int numberOfExpressions);
  ~ExpressionMatch();
  Expression * expression(int i);
  int numberOfExpressions();
  ExpressionMatch& operator=(ExpressionMatch&& other);
private:
  Expression ** m_expressions;
  int m_numberOfExpressions;
};

#endif
