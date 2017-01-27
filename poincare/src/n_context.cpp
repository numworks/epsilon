#include <poincare/n_context.h>

NContext::NContext(::Context * parentContext) :
  m_nValue(Float(0.0f)),
  m_parentContext(parentContext)
{
}

void NContext::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  if (symbol->name() == 'n') {
  	m_nValue = Float((int)expression->approximate(*m_parentContext));
  } else {
    m_parentContext->setExpressionForSymbolName(expression, symbol);
  }
}

const Expression * NContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == 'n') {
    return &m_nValue;
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

