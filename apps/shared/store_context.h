#ifndef SHARED_STORE_CONTEXT_H
#define SHARED_STORE_CONTEXT_H

#include <poincare/expression.h>
#include <poincare/context_with_parent.h>
#include <poincare/symbol.h>
#include "double_pair_store.h"
#include <cmath>

namespace Shared {

class StoreContext : public Poincare::ContextWithParent {
public:
  StoreContext(Shared::DoublePairStore * store, Context * parentContext) :
    Poincare::ContextWithParent(parentContext),
    m_store(store),
    m_seriesPairIndex(-1)
  {}
  void setSeriesPairIndex(int j) { m_seriesPairIndex = j; }
protected:
  Shared::DoublePairStore * m_store;
  int m_seriesPairIndex;
};

}

#endif
