#pragma once

#include <assert.h>

#include "symbol_context.h"

namespace Poincare {

// If parentContext is nullptr, this context will behave like an empty context
class ContextWithParent : public SymbolContext {
 public:
  ContextWithParent(const SymbolContext* parentContext)
      : m_parentContext(parentContext) {}

  // Context
  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const override {
    return m_parentContext ? m_parentContext->expressionForUserNamed(symbol)
                           : nullptr;
  }

  UserNamedType expressionTypeForIdentifier(
      std::string_view identifier) const override {
    return m_parentContext
               ? m_parentContext->expressionTypeForIdentifier(identifier)
               : UserNamedType::None;
  }

 private:
  const SymbolContext* m_parentContext;
};

}  // namespace Poincare
