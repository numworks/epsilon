#include <poincare/pool_variable_context.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/symbol.h>

namespace Poincare {

SymbolContext::SymbolType PoolVariableContext::symbolTypeForIdentifier(
    std::string_view identifier) const {
  if ((m_name != nullptr) && (identifier == m_name)) {
    return SymbolType::Symbol;
  }
  return ContextWithParent::symbolTypeForIdentifier(identifier);
}

const Internal::Tree* PoolVariableContext::expressionForSymbol(
    const Internal::Tree* symbol) const {
  if ((m_name != nullptr) &&
      (std::string_view(Internal::Symbol::GetName(symbol)) == m_name)) {
    if (symbol->isUserSymbol()) {
      return m_value;
    }
    return Internal::KTrees::KUndef;
  }
  return ContextWithParent::expressionForSymbol(symbol);
}

}  // namespace Poincare
