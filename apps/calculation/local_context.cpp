#include "local_context.h"
#include <string.h>
using namespace Poincare;

namespace Calculation {

LocalContext::LocalContext(GlobalContext * parentContext, CalculationStore * calculationStore) :
  m_calculationStore(calculationStore),
  m_parentContext(parentContext)
{
}

Evaluation<double> * LocalContext::ansValue() {
  if (m_calculationStore->numberOfCalculations() == 0) {
    return m_parentContext->defaultExpression();
  }
  Calculation * lastCalculation = m_calculationStore->calculationAtIndex(m_calculationStore->numberOfCalculations()-1);
  return lastCalculation->output(m_parentContext);
}

void LocalContext::setExpressionForSymbolName(Expression * expression, const Symbol * symbol) {
  if (symbol->name() != Symbol::SpecialSymbols::Ans) {
    m_parentContext->setExpressionForSymbolName(expression, symbol);
  }
}

const Expression * LocalContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == Symbol::SpecialSymbols::Ans) {
    return ansValue();
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

}
