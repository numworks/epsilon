#include <poincare/context.h>
#include <assert.h>

Context::Context()
{
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

void Context::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  int index = symbolIndex(symbol);
  m_expressions[index] = expression;
}
