#include <poincare/variable_context.h>
#include <poincare/preferences.h>
#include <assert.h>

namespace Poincare {

VariableContext::VariableContext(char name, Context * parentContext) :
  m_name(name),
  m_value(Complex::Float(0.0f)),
  m_parentContext(parentContext)
{
}

void VariableContext::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  if (symbol->name() == m_name) {
    assert(expression->type() == Expression::Type::Complex);
    /* WARNING: We assume that the evaluation of expression is a reel */
    m_value = Complex::Float(expression->approximate(*m_parentContext, Preferences::sharedPreferences()->angleUnit()));
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
