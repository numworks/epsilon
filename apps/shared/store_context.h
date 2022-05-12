#ifndef SHARED_STORE_CONTEXT_H
#define SHARED_STORE_CONTEXT_H

#include <poincare/context_with_parent.h>
#include "double_pair_store.h"

namespace Shared {

class StoreContext : public Poincare::ContextWithParent {
public:
  StoreContext(Shared::DoublePairStore * store, Context * parentContext) :
    Poincare::ContextWithParent(parentContext),
    m_store(store)
  {}

  Poincare::Context::SymbolAbstractType expressionTypeForIdentifier(const char * identifier, int length) override {
  return m_store->isColumnName(identifier, length) ? Poincare::Context::SymbolAbstractType::Symbol : Poincare::ContextWithParent::expressionTypeForIdentifier(identifier, length);
}

private:
  Shared::DoublePairStore * m_store;
};

}

#endif
