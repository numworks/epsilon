#include <poincare/variable_context.h>
#include <poincare/preferences.h>
#include <poincare/symbol.h>

#include <cmath>

namespace Poincare {

VariableContext::VariableContext(const char * name, Context * parentContext) :
  m_name(name),
  m_value(),
  m_parentContext(parentContext)
{
}

template<typename T>
void VariableContext::setApproximationForVariable(T value) {
  m_value = Float<T>(value);
}

void VariableContext::setExpressionForSymbolName(const Expression & expression, const char * symbolName, Context & context) {
  if (strcmp(symbolName, m_name) == 0) {
    if (expression.isUninitialized()) {
      return;
    }
    m_value = expression.clone();
  } else {
    m_parentContext->setExpressionForSymbolName(expression, symbolName, context);
  }
}

const Expression VariableContext::expressionForSymbol(const Symbol & symbol) {
  if (strcmp(symbol.name(), m_name) == 0) {
    return m_value;
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

template void VariableContext::setApproximationForVariable(float);
template void VariableContext::setApproximationForVariable(double);

}
