#include <poincare/global_context.h>
#include <assert.h>
#include <math.h>

GlobalContext::GlobalContext() :
  m_pi(Float(M_PI))
{
  for (int i = 0; i < k_maxNumberOfScalarExpressions; i++) {
    m_expressions[i] = nullptr;
  }
}

Float * GlobalContext::defaultExpression() {
  static Float * defaultExpression = new Float(0);
  return defaultExpression;
}

int GlobalContext::symbolIndex(const Symbol * symbol) const {
  int index = symbol->name() - 'A';
  return index;
}

const Expression * GlobalContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == Symbol::SpecialSymbols::Pi) {
    return &m_pi;
  }
  int index = symbolIndex(symbol);
  if (index < 0 || index >= k_maxNumberOfScalarExpressions) {
    return nullptr;
  }
  if (m_expressions[index] == nullptr) {
    return defaultExpression();
  }
  return m_expressions[index];
}

void GlobalContext::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  int index = symbolIndex(symbol);
  m_expressions[index] = expression;
}
