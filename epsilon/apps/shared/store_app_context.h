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

 protected:
  /* Force the column names to be recognized as lists symbols, even if they are
   * not defined yet */
  Poincare::SymbolContext::SymbolDescription symbolDescriptionForIdentifier(
      std::string_view identifier) const override {
    SymbolDescription parentDesc =
        Poincare::ContextWithParent::symbolDescriptionForIdentifier(identifier);
    if (m_store->isColumnName(identifier)) {
      return {Poincare::SymbolContext::SymbolType::List, parentDesc.expression};
    }
    return parentDesc;
  }

 private:
  Shared::DoublePairStore* m_store;
};

}  // namespace Shared
