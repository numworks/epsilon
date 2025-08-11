#include <omg/utf8_helper.h>
#include <poincare/pool_variable_context.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/symbol.h>

#include <cmath>

namespace Poincare {

Context::UserNamedType PoolVariableContext::expressionTypeForIdentifier(
    const char* identifier, int length) const {
  if (UTF8Helper::CompareNonNullTerminatedStringWithNullTerminated(
          identifier, length, m_name) == 0) {
    return UserNamedType::Symbol;
  }
  return ContextWithParent::expressionTypeForIdentifier(identifier, length);
}

const Internal::Tree* PoolVariableContext::expressionForUserNamed(
    const Internal::Tree* symbol) const {
  if (m_name != nullptr &&
      strcmp(Internal::Symbol::GetName(symbol), m_name) == 0) {
    if (symbol->isUserSymbol()) {
      return m_value;
    }
    return Internal::KTrees::KUndef;
  }
  return ContextWithParent::expressionForUserNamed(symbol);
}

}  // namespace Poincare
