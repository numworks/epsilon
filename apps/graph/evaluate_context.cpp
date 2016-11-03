#include "evaluate_context.h"
#include <string.h>

namespace Graph {

EvaluateContext::EvaluateContext(::Context * parentContext) :
  m_xValue(Float(0.0f)),
  m_context(parentContext)
{
}

void EvaluateContext::setOverridenValueForSymbolX(float f) {
  m_xValue = Float(f);
}

const Expression * EvaluateContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == 'x') {
    return &m_xValue;
  } else {
    return m_context->expressionForSymbol(symbol);
  }
}

}
