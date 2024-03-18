#include <poincare/preferences.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>

#include <cmath>

namespace Poincare {

template <typename T>
void VariableContext::setApproximationForVariable(T value) {
  m_value = Float<T>::Builder(value);
}

Context::SymbolAbstractType VariableContext::expressionTypeForIdentifier(
    const char* identifier, int length) {
  if (UTF8Helper::CompareNonNullTerminatedStringWithNullTerminated(
          identifier, length, m_name) == 0) {
    return m_value.isUninitialized() ? SymbolAbstractType::None
                                     : SymbolAbstractType::Symbol;
  }
  return ContextWithParent::expressionTypeForIdentifier(identifier, length);
}

bool VariableContext::setExpressionForSymbolAbstract(
    const Expression& expression, const SymbolAbstract& symbol) {
  if (m_name != nullptr && strcmp(symbol.name(), m_name) == 0) {
    assert(symbol.type() == ExpressionNode::Type::Symbol);
    if (expression.isUninitialized()) {
      return false;
    }
    m_value = expression.clone();
    return true;
  }
  return ContextWithParent::setExpressionForSymbolAbstract(expression, symbol);
}

const Expression VariableContext::protectedExpressionForSymbolAbstract(
    const SymbolAbstract& symbol, bool clone,
    ContextWithParent* lastDescendantContext) {
  if (m_name != nullptr && strcmp(symbol.name(), m_name) == 0) {
    if (symbol.type() == ExpressionNode::Type::Symbol) {
      return clone ? m_value.clone() : m_value;
    }
    return Undefined::Builder();
  }
  return ContextWithParent::protectedExpressionForSymbolAbstract(
      symbol, clone, lastDescendantContext);
}

template void VariableContext::setApproximationForVariable(float);
template void VariableContext::setApproximationForVariable(double);

}  // namespace Poincare
