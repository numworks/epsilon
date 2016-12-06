#include "evaluate_context.h"
#include <string.h>

namespace Probability {

EvaluateContext::EvaluateContext(::Context * parentContext) :
  m_tValue(Float(0.0f)),
  m_firstParameterValue(Float(0.0f)),
  m_secondParameterValue(Float(0.0f)),
  m_context(parentContext)
{
}

void EvaluateContext::setOverridenValueForSymbolT(float f) {
  m_tValue = Float(f);
}

void EvaluateContext::setOverridenValueForFirstParameter(float f) {
  m_firstParameterValue = Float(f);
}

void EvaluateContext::setOverridenValueForSecondParameter(float f) {
  m_secondParameterValue = Float(f);
}

const Expression * EvaluateContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == 't') {
    return &m_tValue;
  }
  if (symbol->name() == Symbol::SpecialSymbols::p1) {
    return &m_firstParameterValue;
  }
  if (symbol->name() == Symbol::SpecialSymbols::p2) {
    return &m_secondParameterValue;
  }
  return m_context->expressionForSymbol(symbol);
}

}
