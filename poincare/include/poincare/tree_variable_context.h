#pragma once

#include "context_with_parent.h"

namespace Poincare {

/* TODO: this context is used only by the parser, rework it ? */

class TreeVariableContext : public ContextWithParent {
 public:
  TreeVariableContext(const char* name, const Internal::Tree* value,
                      const SymbolContext* parentContext = nullptr)
      : ContextWithParent(parentContext), m_name(name), m_value(value) {}
  TreeVariableContext(const char* name = nullptr,
                      const SymbolContext* parentContext = nullptr)
      : TreeVariableContext(name, nullptr, parentContext) {}

 protected:
  // Symbol Context override
  SymbolDescription symbolDescriptionForIdentifier(
      std::string_view identifier) const override {
    if ((m_name != nullptr) && (identifier == m_name)) {
      return {SymbolType::Symbol, m_value};
    }
    return ContextWithParent::symbolDescriptionForIdentifier(identifier);
  }

 private:
  const char* m_name;
  const Internal::Tree* m_value;
};

}  // namespace Poincare
