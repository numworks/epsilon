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
  const Poincare::Expression expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, float unknownSymbolValue = NAN) override;
  Poincare::Context::SymbolAbstractType expressionTypeForIdentifier(const char * identifier, int length) override;

protected:
  virtual bool isSymbol(const char * name) const = 0;
  virtual int getSymbolColumn(const char * name) const = 0;

  Shared::DoublePairStore * m_store;
  int m_seriesPairIndex;
};

}

#endif
