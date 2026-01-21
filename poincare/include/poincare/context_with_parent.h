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
  const Internal::Tree* expressionForSymbol(
      const Internal::Tree* symbol) const override {
    return m_parentContext ? m_parentContext->expressionForSymbol(symbol)
                           : nullptr;
  }

  SymbolType symbolTypeForIdentifier(
      std::string_view identifier) const override {
    return m_parentContext
               ? m_parentContext->symbolTypeForIdentifier(identifier)
               : SymbolType::None;
  }

#if POINCARE_UNIT
  bool useStrictUnitLayout() const override {
    return m_parentContext ? m_parentContext->useStrictUnitLayout()
                           : SymbolContext::useStrictUnitLayout();
  }
#endif

 private:
  const SymbolContext* m_parentContext;
};

#if POINCARE_UNIT
class StrictUnitContext : public ContextWithParent {
 public:
  using ContextWithParent::ContextWithParent;
  bool useStrictUnitLayout() const override { return true; }
};
#endif

}  // namespace Poincare
