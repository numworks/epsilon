#include <poincare/context.h>
#include <assert.h>

Context::Context()
{
  for (int i = 0; i < k_maxNumberOfExpressions; i++) {
    m_expressions[i] = nullptr;
  }
}

int Context::symbolIndex(const Symbol * symbol) const {
  int index = symbol->name() - 'A';
  assert(index < k_maxNumberOfExpressions);
  return index;
}

const Expression * Context::expressionForSymbol(const Symbol * symbol) {
  int index = symbolIndex(symbol);
  return m_expressions[index];
}

const Expression * Context::scalarExpressionForIndex(int index) {
  assert(index < k_maxNumberOfExpressions);
  return m_expressions[index];
}

void Context::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  int index = symbolIndex(symbol);
  m_expressions[index] = expression;
}
