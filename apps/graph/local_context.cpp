#include "local_context.h"
#include <string.h>

namespace Graph {

LocalContext::LocalContext(::Context * parentContext) :
  m_xValue(Float(0.0f)),
  m_parentContext(parentContext)
{
}

void LocalContext::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  if (symbol->name() == 'x') {
  	m_xValue = Float(expression->approximate(*m_parentContext));
  } else {
    m_parentContext->setExpressionForSymbolName(expression, symbol);
  }
}

const Expression * LocalContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == 'x') {
    return &m_xValue;
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

}
