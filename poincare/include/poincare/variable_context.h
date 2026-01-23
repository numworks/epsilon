#pragma once

#include <poincare/src/memory/tree.h>

#include "context_with_parent.h"

namespace Poincare {

// TreeWrapper must be convertible to const Internal::Tree*
template <typename TreeWrapper>
class VariableContext : public ContextWithParent {
 public:
  VariableContext(const char* name, TreeWrapper value,
                  const SymbolContext* parentContext = nullptr)
      : ContextWithParent(parentContext), m_name(name), m_value(value) {}

 protected:
  // Symbol Context override

  SymbolDescription symbolDescriptionForIdentifier(
      std::string_view identifier) const override {
    if ((m_name != nullptr) && (identifier == m_name)) {
      return {SymbolType::Variable,
              static_cast<const Internal::Tree*>(m_value)};
    }
    return ContextWithParent::symbolDescriptionForIdentifier(identifier);
  }

 private:
  const char* m_name;
  TreeWrapper m_value;
};

using TreeVariableContext = VariableContext<const Internal::Tree*>;

}  // namespace Poincare
