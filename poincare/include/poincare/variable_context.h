#pragma once

#include <poincare/src/memory/tree.h>

#include "context_with_parent.h"

namespace Poincare {

// TreeWrapper must be convertible to const Internal::Tree*
template <typename TreeWrapper>
class VariableContext : public ContextWithParent {
 public:
  VariableContext(const char* name, TreeWrapper variable,
                  const SymbolContext* parentContext = nullptr)
      : ContextWithParent(parentContext), m_name(name), m_variable(variable) {}

 protected:
  // Symbol Context override

  SymbolDescription symbolDescriptionForIdentifier(
      std::string_view identifier) const override {
    if ((m_name != nullptr) && (identifier == m_name)) {
      return {SymbolType::Symbol,
              static_cast<const Internal::Tree*>(m_variable)};
    }
    return ContextWithParent::symbolDescriptionForIdentifier(identifier);
  }

 private:
  const char* m_name;
  TreeWrapper m_variable;
};

using TreeVariableContext = VariableContext<const Internal::Tree*>;

}  // namespace Poincare
