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
  m_value = Float<T>::Builder(value);
}

void VariableContext::setExpressionForSymbol(const Expression & expression, const SymbolAbstract & symbol, Context * context) {
  if (strcmp(symbol.name(), m_name) == 0) {
    if (expression.isUninitialized()) {
      return;
    }
    m_value = expression.clone();
  } else {
    m_parentContext->setExpressionForSymbol(expression, symbol, context);
  }
}

const Expression VariableContext::expressionForSymbol(const SymbolAbstract & symbol, bool clone) {
  if (strcmp(symbol.name(), m_name) == 0) {
    return clone ? m_value.clone() : m_value;
  } else {
    return m_parentContext->expressionForSymbol(symbol, clone);
  }
}

template void VariableContext::setApproximationForVariable(float);
template void VariableContext::setApproximationForVariable(double);

}
