#include "local_context.h"
#include <string.h>
using namespace Poincare;

namespace Calculation {

LocalContext::LocalContext(GlobalContext * parentContext, CalculationStore * calculationStore) :
  m_calculationStore(calculationStore),
  m_parentContext(parentContext)
{
}

Evaluation * LocalContext::ansValue() {
  if (m_calculationStore->numberOfCalculations() == 0) {
    return m_parentContext->defaultExpression();
  }
  Calculation * lastCalculation = m_calculationStore->calculationAtIndex(m_calculationStore->numberOfCalculations()-1);
  return lastCalculation->output(m_parentContext);
}

void LocalContext::setExpressionForSymbolName(Evaluation * expression, const Symbol * symbol) {
  if (symbol->name() != Symbol::SpecialSymbols::Ans) {
    m_parentContext->setExpressionForSymbolName(expression, symbol);
  }
}

const Evaluation * LocalContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == Symbol::SpecialSymbols::Ans) {
    return ansValue();
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

}
