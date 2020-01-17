#include <poincare/variable_context.h>
#include <poincare/preferences.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>

#include <cmath>

namespace Poincare {

template<typename T>
void VariableContext::setApproximationForVariable(T value) {
  m_value = Float<T>::Builder(value);
}

void VariableContext::setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol) {
  if (m_name != nullptr && strcmp(symbol.name(), m_name) == 0) {
    assert(symbol.type() == ExpressionNode::Type::Symbol);
    if (expression.isUninitialized()) {
      return;
    }
    m_value = expression.clone();
  } else {
    return ContextWithParent::setExpressionForSymbolAbstract(expression, symbol);
  }
}

const Expression VariableContext::expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone) {
  if (m_name != nullptr && strcmp(symbol.name(), m_name) == 0) {
    if (symbol.type() == ExpressionNode::Type::Symbol) {
      return clone ? m_value.clone() : m_value;
    }
    return Undefined::Builder();
  } else {
    return ContextWithParent::expressionForSymbolAbstract(symbol, clone);
  }
}

template void VariableContext::setApproximationForVariable(float);
template void VariableContext::setApproximationForVariable(double);

}
