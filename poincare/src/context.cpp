#include <poincare/context.h>
#include <assert.h>

Context::Context()
{
  for (int i = 0; i < k_maxNumberOfExpressions; i++) {
    m_expressions[i] = nullptr;
  }
}

Float * Context::defaultExpression() {
  static Float * defaultExpression = new Float(0);
  return defaultExpression;
}

int Context::symbolIndex(const Symbol * symbol) const {
  int index = symbol->name() - 'A';
  return index;
}

const Expression * Context::expressionForSymbol(const Symbol * symbol) {
  int index = symbolIndex(symbol);
  if (index < 0 || index >= k_maxNumberOfExpressions) {
    return nullptr;
  }
  if (m_expressions[index] == nullptr) {
    return defaultExpression();
  }
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
