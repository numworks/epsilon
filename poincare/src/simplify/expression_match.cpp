#include "expression_match.h"

extern "C" {
#include <assert.h>
#include <stdlib.h>
}

ExpressionMatch::ExpressionMatch() {
  m_numberOfExpressions = 0;
  m_expressions = nullptr;
}

ExpressionMatch::ExpressionMatch(Expression ** expressions, int numberOfExpressions) {
  m_numberOfExpressions = numberOfExpressions;
  m_expressions = (Expression**) malloc(m_numberOfExpressions * sizeof(ExpressionMatch*));
  for (int i(0); i<numberOfExpressions; i++) {
    m_expressions[i] = expressions[i];
  }
}

ExpressionMatch::~ExpressionMatch() {
  if (m_numberOfExpressions != 0) {
    free(m_expressions);
  }
}

Expression * ExpressionMatch::expression(int i) {
  assert(i < m_numberOfExpressions);
  return m_expressions[i];
}

int ExpressionMatch::numberOfExpressions() {
  return m_numberOfExpressions;
}

ExpressionMatch& ExpressionMatch::operator=(ExpressionMatch&& other) {
  m_numberOfExpressions = other.m_numberOfExpressions;
  other.m_numberOfExpressions = 0; // Here we make sure that the memory containing the expressions is not freed.
  m_expressions = other.m_expressions;
  return *this;
}
