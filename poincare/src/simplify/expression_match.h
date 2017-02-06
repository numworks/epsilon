#ifndef POINCARE_SIMPLIFY_EXPRESSION_MATCH_H
#define POINCARE_SIMPLIFY_EXPRESSION_MATCH_H

#include <poincare/expression.h>
extern "C" {
#include <stdint.h>
}

namespace Poincare {

class ExpressionMatch {
public:
  ExpressionMatch();
  ExpressionMatch(const Expression ** expressions, int numberOfExpressions);
  ~ExpressionMatch();
  const Expression * expression(int i);
  int numberOfExpressions();
  ExpressionMatch& operator=(ExpressionMatch&& other);
private:
  const Expression ** m_expressions;
  int m_numberOfExpressions;
};

}

#endif
