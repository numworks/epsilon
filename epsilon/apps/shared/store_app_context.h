#pragma once

#include <poincare/context_with_parent.h>

#include "double_pair_store.h"

namespace Shared {

/* This Context is used in Apps which are centered on a double column table
 * (Statistics and Regression) */
class StoreAppContext : public Poincare::ContextWithParent {
 public:
  StoreAppContext(Shared::DoublePairStore* store,
                  const SymbolContext* parentContext)
      : Poincare::ContextWithParent(parentContext), m_store(store) {}

  Poincare::SymbolContext::UserNamedType expressionTypeForIdentifier(
      std::string_view identifier) const override {
    return m_store->isColumnName(identifier)
               ? Poincare::SymbolContext::UserNamedType::List
               : Poincare::ContextWithParent::expressionTypeForIdentifier(
                     identifier);
  }

 private:
  Shared::DoublePairStore* m_store;
};

}  // namespace Shared
