#include <poincare/x_context.h>

XContext::XContext(::Context * parentContext) :
  m_xValue(Float(0.0f)),
  m_parentContext(parentContext)
{
}

void XContext::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  if (symbol->name() == 'x') {
  	m_xValue = Float(expression->approximate(*m_parentContext));
  } else {
    m_parentContext->setExpressionForSymbolName(expression, symbol);
  }
}

const Expression * XContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == 'x') {
    return &m_xValue;
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

