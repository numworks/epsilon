#pragma once

#include <poincare/context_with_parent.h>

#include "double_pair_store.h"

namespace Shared {

class StoreContext : public Poincare::ContextWithParent {
 public:
  StoreContext(Shared::DoublePairStore* store, const Context* parentContext)
      : Poincare::ContextWithParent(parentContext), m_store(store) {}

  Poincare::Context::UserNamedType expressionTypeForIdentifier(
      std::string_view identifier) const override {
    return m_store->isColumnName(identifier)
               ? Poincare::Context::UserNamedType::List
               : Poincare::ContextWithParent::expressionTypeForIdentifier(
                     identifier);
  }

 private:
  Shared::DoublePairStore* m_store;
};

}  // namespace Shared
