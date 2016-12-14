#include "local_context.h"
#include <string.h>

namespace Probability {

LocalContext::LocalContext(::Context * parentContext) :
  m_tValue(Float(0.0f)),
  m_aValue(Float(0.0f)),
  m_bValue(Float(0.0f)),
  m_parentContext(parentContext)
{
}


void LocalContext::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  // TODO: Add syntax error if expression is a matrix?
  if (symbol->name() == 't') {
    m_tValue = Float(expression->approximate(*m_parentContext));
    return;
  }
  if (symbol->name() == 'a') {
    m_aValue = Float(expression->approximate(*m_parentContext));
    return;
  }
  if (symbol->name() == 'b') {
    m_bValue = Float(expression->approximate(*m_parentContext));
    return;
  }
  m_parentContext->setExpressionForSymbolName(expression, symbol);
}

const Expression * LocalContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == 't') {
    return &m_tValue;
  }
  if (symbol->name() == 'a') {
    return &m_aValue;
  }
  if (symbol->name() == 'b') {
    return &m_bValue;
  }
  return m_parentContext->expressionForSymbol(symbol);
}

}
