#pragma once

#include <assert.h>

#include "context.h"

namespace Poincare {

// If parentContext is nullptr, this context will behave like an empty context
class ContextWithParent : public Context {
 public:
  ContextWithParent(const Context* parentContext)
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
  const Context* m_parentContext;
};

}  // namespace Poincare
