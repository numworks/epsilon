#ifndef SHARED_STORE_CONTEXT_H
#define SHARED_STORE_CONTEXT_H

#include <poincare/expression.h>
#include <poincare/context.h>
#include <poincare/symbol.h>
#include "double_pair_store.h"
#include <cmath>

namespace Shared {

class StoreContext : public Poincare::Context {
public:
  StoreContext(Shared::DoublePairStore * store) :
    Poincare::Context(),
    m_store(store),
    m_seriesPairIndex(-1),
    m_parentContext(nullptr)
  {}
  void setParentContext(Poincare::Context * parentContext) { m_parentContext = parentContext; }
  void setSeriesPairIndex(int j) { m_seriesPairIndex = j; }
  void setExpressionForSymbolAbstract(const Poincare::Expression & expression, const Poincare::SymbolAbstract & symbol, Poincare::Context * context) override;
protected:
  Shared::DoublePairStore * m_store;
  int m_seriesPairIndex;
  Poincare::Context * m_parentContext;
};

}

#endif
