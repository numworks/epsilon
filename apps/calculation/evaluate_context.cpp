#include "evaluate_context.h"
#include <string.h>

namespace Calculation {

EvaluateContext::EvaluateContext(::Context * parentContext, CalculationStore * calculationStore) :
  m_ansValue(Float(0.0f)),
  m_calculationStore(calculationStore),
  m_context(parentContext)
{
}

Float * EvaluateContext::ansValue() {
  Calculation * lastCalculation = m_calculationStore->calculationAtIndex(m_calculationStore->numberOfCalculations()-1);
  m_ansValue = Float(lastCalculation->evaluation());
  return &m_ansValue;
}

const Expression * EvaluateContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == Symbol::SpecialSymbols::Ans) {
    return ansValue();
  } else {
    return m_context->expressionForSymbol(symbol);
  }
}

}
