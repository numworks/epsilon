#include <poincare/variable_context.h>
#include <assert.h>

namespace Poincare {

VariableContext::VariableContext(char name, Context * parentContext) :
  m_name(name),
  m_value(Complex(0.0f)),
  m_parentContext(parentContext)
{
}

void VariableContext::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  if (symbol->name() == m_name) {
    assert(expression->type() == Expression::Type::Complex);
    /* WARNING: We assume that the evaluation of expression is a reel */
  	m_value = Complex(expression->approximate(*m_parentContext));
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

}
