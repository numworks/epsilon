#include "local_context.h"
#include <string.h>
using namespace Poincare;

namespace Calculation {

LocalContext::LocalContext(GlobalContext * parentContext, CalculationStore * calculationStore) :
  m_calculationStore(calculationStore),
  m_parentContext(parentContext)
{
}

Expression * LocalContext::ansValue() {
  if (m_calculationStore->numberOfCalculations() == 0) {
    return defaultExpression();
  }
  Calculation * lastCalculation = m_calculationStore->calculationAtIndex(m_calculationStore->numberOfCalculations()-1);
  return lastCalculation->exactOutput(m_parentContext);
}

Expression * LocalContext::defaultExpression() {
  static Rational defaultExpression(0);
  return &defaultExpression;
}

void LocalContext::setExpressionForSymbolName(const Expression * expression, const Symbol * symbol, Context & context) {
  if (symbol->name() != Symbol::SpecialSymbols::Ans) {
    m_parentContext->setExpressionForSymbolName(expression, symbol, context);
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
