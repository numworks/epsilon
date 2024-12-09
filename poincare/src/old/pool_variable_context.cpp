#include <poincare/old/pool_variable_context.h>
#include <poincare/old/symbol.h>
#include <poincare/old/undefined.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/symbol.h>

#include <cmath>

namespace Poincare {

template <typename T>
void PoolVariableContext::setApproximationForVariable(T value) {
  m_value = Expression::Builder<T>(value);
}

Context::SymbolAbstractType PoolVariableContext::expressionTypeForIdentifier(
    const char* identifier, int length) {
  if (UTF8Helper::CompareNonNullTerminatedStringWithNullTerminated(
          identifier, length, m_name) == 0) {
    return SymbolAbstractType::Symbol;
  }
  return ContextWithParent::expressionTypeForIdentifier(identifier, length);
}

bool PoolVariableContext::setExpressionForUserNamed(
    const Internal::Tree* expression, const Internal::Tree* symbol) {
  if (m_name != nullptr &&
      strcmp(Internal::Symbol::GetName(symbol), m_name) == 0) {
    assert(symbol->isUserSymbol());
    if (!expression) {
      return false;
    }
    m_value = Expression::Builder(expression->cloneTree());
    return true;
  }
  return ContextWithParent::setExpressionForUserNamed(expression, symbol);
}

const Internal::Tree* PoolVariableContext::expressionForUserNamed(
    const Internal::Tree* symbol) {
  if (m_name != nullptr &&
      strcmp(Internal::Symbol::GetName(symbol), m_name) == 0) {
    if (symbol->isUserSymbol()) {
      return m_value;
    }
    return Internal::KTrees::KUndef;
  }
  return ContextWithParent::expressionForUserNamed(symbol);
}

template void PoolVariableContext::setApproximationForVariable(float);
template void PoolVariableContext::setApproximationForVariable(double);

}  // namespace Poincare
