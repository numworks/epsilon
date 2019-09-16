#include <poincare/variable_context.h>
#include <poincare/preferences.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>

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

void VariableContext::setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol) {
  if (strcmp(symbol.name(), m_name) == 0) {
    assert(symbol.type() == ExpressionNode::Type::Symbol);
    if (expression.isUninitialized()) {
      return;
    }
    m_value = expression.clone();
  } else {
    m_parentContext->setExpressionForSymbolAbstract(expression, symbol);
  }
}

const Expression VariableContext::expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone) {
  if (strcmp(symbol.name(), m_name) == 0) {
    if (symbol.type() == ExpressionNode::Type::Symbol) {
      return clone ? m_value.clone() : m_value;
    }
    return Undefined::Builder();
  } else {
    return m_parentContext->expressionForSymbolAbstract(symbol, clone);
  }
}

template void VariableContext::setApproximationForVariable(float);
template void VariableContext::setApproximationForVariable(double);

}
