#include <poincare/variable_context.h>

VariableContext::VariableContext(char name, ::Context * parentContext) :
  m_name(name),
  m_value(Float(0.0f)),
  m_parentContext(parentContext)
{
}

void VariableContext::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  if (symbol->name() == m_name) {
    /* WARNING: we here assume that the expression does not content any function
     * whose evaluation depends on the angle unit */
  	m_value = Float(expression->approximate(*m_parentContext));
  } else {
    m_parentContext->setExpressionForSymbolName(expression, symbol);
  }
}

const Expression * VariableContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == m_name) {
    return &m_value;
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

