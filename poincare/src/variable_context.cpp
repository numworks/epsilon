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

const Expression VariableContext::expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone, float unknownSymbolValue ) {
  if (m_name != nullptr && strcmp(symbol.name(), m_name) == 0) {
    if (symbol.type() == ExpressionNode::Type::Symbol) {
      return clone ? m_value.clone() : m_value;
    }
    return Undefined::Builder();
  } else {
    Symbol unknownSymbol = Symbol::Builder(UCodePointUnknown);
    if (m_name != nullptr && strcmp(m_name, unknownSymbol.name()) == 0) {
      float previousUnknownSymbolValue = unknownSymbolValue;
      unknownSymbolValue = m_value.approximateToScalar<float>(this, Preferences::sharedPreferences()->complexFormat(), Preferences::sharedPreferences()->angleUnit(), true);
      /* If previousUnknownSymbolValue is not NaN, it means unknownSymbol was
       * attributed a value from a previous context. It can happen when a
       * derivative is formed on an expression with UCodePointUnknown as both
       * symbol and symbol value (such as a graph derivative). It isn't an issue
       * as long as both symbols had the same value. */
      assert(std::isnan(previousUnknownSymbolValue) || previousUnknownSymbolValue == unknownSymbolValue);
    }
    return ContextWithParent::expressionForSymbolAbstract(symbol, clone, unknownSymbolValue);
  }
}

template void VariableContext::setApproximationForVariable(float);
template void VariableContext::setApproximationForVariable(double);

}
