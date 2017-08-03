#include <poincare/variable_context.h>
#include <poincare/preferences.h>
#include <assert.h>

namespace Poincare {

VariableContext::VariableContext(char name, Context * parentContext) :
  m_name(name),
  m_value(Complex::Float(NAN)),
  m_parentContext(parentContext)
{
}

void VariableContext::setExpressionForSymbolName(Evaluation * expression, const Symbol * symbol) {
  if (symbol->name() == m_name) {
    assert(expression->numberOfOperands() == 1);
    /* WARNING: We assume that the evaluation of expression is a real */
    m_value = Complex::Float(expression->toFloat());
  } else {
    m_parentContext->setExpressionForSymbolName(expression, symbol);
  }
}

const Evaluation * VariableContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == m_name) {
    return &m_value;
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

}
