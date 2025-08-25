#include <poincare/preferences.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/tree_variable_context.h>

namespace Poincare {

Context::UserNamedType TreeVariableContext::expressionTypeForIdentifier(
    std::string_view identifier) const {
  if ((m_name != nullptr) && (identifier == m_name)) {
    return UserNamedType::Symbol;
  }
  return ContextWithParent::expressionTypeForIdentifier(identifier);
}

const Internal::Tree* TreeVariableContext::expressionForUserNamed(
    const Internal::Tree* symbol) const {
  if ((m_name != nullptr) &&
      (std::string_view(Internal::Symbol::GetName(symbol)) == m_name)) {
    if (symbol->isUserSymbol()) {
      return m_value;
    }
    return Internal::KTrees::KUndef;
  }
  return ContextWithParent::expressionForUserNamed(symbol);
}

}  // namespace Poincare
